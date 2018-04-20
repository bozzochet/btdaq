#ifndef DATA_VME
#define DATA_VME

//******************************************************************************************************************
//******************************************************************************************************************
//***************************** VME DEFINITIONS ********************************************************************
//******************************************************************************************************************
//******************************************************************************************************************
#define VME_A16D32            "/dev/vmedrv16d32"    // Dev to access VME addr=16bit and data=32bit
#define VME_A24D16            "/dev/vmedrv24d16"    // Dev to access VME addr=24bit and data=16bit
#define VME_A32D32            "/dev/vmedrv32d32"    // Dev to access VME addr=32bit and data=32bit
#define VME_A32D32_DMA        "/dev/vmedrv32d32dma" // Dev to access VME addr=32bit and data=32bit in BLT
#define VME_A24D32_DMA        "/dev/vmedrv24d32dma" // Dev to access VME addr=24bit and data=32bit in BLT
#define OUTDIR                "./data/"             // Directory for storing data files
#define EVEFILESUFFIX          "part"               // Suffix for particle data files
#define PEDFILESUFFIX          "pede"               // Suffix for pedestal files
#define EV_SHOW_REFRESH       10                    // Number of ev for acquisition screen refresh

//******************************************************************************************************************
//******************************************************************************************************************
//***************************** CAEN V262 DEFINITIONS ************************************************************** 
//******************************************************************************************************************
//******************************************************************************************************************
#define V262_ENABLED              true
#define CAEN_V262_ADDRESS         0x00400000        // BASE VME ADDRESS FOR IN/OUT REGISTER mod. CAEN V262

//--- Define channels for NIM levels  ---
#define V262_MAINRESET1_CH        1
#define V262_MAINRESET2_CH        2
#define V262_BUSY_CH              3
#define V262_PEDE_CH              4

//--- Define channels for NIM signals ---
#define V262_VETORESET_CH         1
#define V262_SOFTTRIG_CH          2
#define V262_CNTRRESET_CH         3
#define V262_START_CH          	  4

//--- Define inputs (lower 4 NIM connectors) for different operations
#define V262_DATA_READY           0x3               //Use only first two channels; data is ready if both are 1 (0011).
#define V262_DATA_READY_MASK      0x3               //Define which channels to consider for DATA READY check (0011)
#define V262_TRIGGER_READY           0x8               //Use last channels; trigger is ready if 1. //eugenio
#define V262_TRIGGER_READY_MASK      0x8               //Define which channels to consider for TRIGGER READY check //eugenio

//******************************************************************************************************************
//******************************************************************************************************************
//***************************** GENERAL ACQ PROTOCOL DEFINITIONS *************************************************** 
//******************************************************************************************************************
//******************************************************************************************************************
#define TAXINWORDS                3847             //Number of words sent by GB to VME (S.Ricciarini)
#define PED_FREQ                  100                //Frequency of pedestal (soft) trigger (in Hz)

//******************************************************************************************************************
//******************************************************************************************************************
//***************************** TAXI BOARD DEFINITIONS ************************************************************* 
//******************************************************************************************************************
//******************************************************************************************************************
#define TAXI_ENABLED              false
#define TAXIREC_BASE_ADDR         0xdaffe000
#define TAXIREC_LIMIT_ADDR        0xdafff000
#define FIFO_FLAG_SECT            "D"

//******************************************************************************************************************
//******************************************************************************************************************
//***************************** LHCF TAXI BOARD DEFINITIONS ******************************************************** 
//******************************************************************************************************************
//******************************************************************************************************************
#define LHCF_TAXI_ENABLED         true
#define LHCF_TAXI_MASK            0xF           // (0xF = 1111 = all ON)
//#define LHCF_VMEREC_BASE_ADDR     0xcaf00000  // Base address
//#define LHCF_VMEREC_LIMIT_ADDR    0xcaff0000
#define LHCF_VMEREC_BASE_ADDR     0x8ef00000    // Base address
#define LHCF_VMEREC_LIMIT_ADDR    0x8eff0000
#define LHCF_VMEREC_GOOD_BITS     32            // Number of good data bits presented on VME output (for 1 word)

#define LHCF_VMEREC_SR_READONLY   0             // Parameter to pass to the methor ReadSR() for simple SR reading;
#define LHCF_VMEREC_SR_RESETALL   1             //   reset SR after reading it;
#define LHCF_VMEREC_SR_RESETBUSY  2             //   reset only the busy signal after reading RS.

//******************************************************************************************************************
//******************************************************************************************************************
//***************************** DEFINITION OF DATA HEADER ********************************************************** 
//******************************************************************************************************************
//******************************************************************************************************************
#include <time.h>

struct VME_Header{
  char Flag;
  int  EvNum;
  long long  Sec;
  long long    nSec;
};

void sig_func(int pippo);
char *GetDateStr();


//******************************************************************************************************************
//******************************************************************************************************************
//***************************** VME DEFINITIONS ********************************************************************
//******************************************************************************************************************
//******************************************************************************************************************

//******************************************************************************************************************
//******************************************************************************************************************
//***************************** CAEN V262 DEFINITIONS ************************************************************* 
//******************************************************************************************************************
//******************************************************************************************************************

//******************************************************************************************************************
//******************************************************************************************************************
//***************************** CAEN V1731 DEFINITIONS *************************************************************
//******************************************************************************************************************
//******************************************************************************************************************
#define V1731_CH_ENABLE_MASK      0xFF   // 0xFF = all channels enabled
#define V1731_CH_CONFIG_MASK      0xEFB5 // All channels (4.12)
#define V1731_CHALL_CFG_ZEROSUPP  0      // 0=disabled; 2=zero length encod (ZLE); 3=full supp based on ampl (4.12)
// ----------------------------------------------------------------------------------------------------
// Definition of quantities for each channels: they must stay on 1 line, otherwise error! (4.3) -------
// ----------------------------------------   Ch1    Ch2    Ch3    Ch4    Ch5    Ch6    Ch7    Ch8   --
static const int V1731_THRESHOLD_ZS[]   ={   0xf7,  0xf7,  0xf7,  0xf7,  0xf7,  0xf7,  0xf7,  0xf7 };   //Thesholds for zero suppression(4.3)
static const int V1731_N_LOOK_BACK_ZS[] ={     10,    10,    10,    10,    10,    10,    10,    10 };   //Backward samples (before start thr)
static const int V1731_N_LOOK_FORW_ZS[] ={     10,    10,    10,    10,    10,    10,    10,    10 };   //Forward  samples (after end thr)
static const int V1731_DAC_VOLT_OFFSET[]={ 0x8000,0x8000,0x8000,0x8000,0x8000,0x8000,0x8000,0x8000 };   //DAC values 16 bit(-0.5V to 0.5V) (4.10) //Not Used
static const int V1731_SIGNAL_POLARITY[]={     -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1 };   //Signal polarity (pos, neg or 0=bipolar )
static const int V1731_CUSTOM_PEDESTAL[]={   0xfa,  0xfa,  0xfa,  0xfa,  0xfa,  0xfa,  0xfa,  0xfa };   //Signal polarity (pos, neg or 0=bipolar ) //Not Used
static const int V1731_AUTOTRIG_THR[]   ={   0xf2,  0xf2,  0xf2,  0xf2,  0xf2,  0xf2,  0xf2,  0xf2 };   //Thesholds for auto trigger(4.3) //Normally Not Used
static const int V1731_AUTOTRIG_N_THR[] ={    0x1,   0x1,   0x1,   0x1,   0x1,   0x1,   0x1,   0x1 };   //# of sampl over thr for auto trigger(4.3)(4.3) //Normally Not Used

// -- General parameters ------------------------------------------------------------------------------
#define V1731_BLT_NEV             0      // # of complete ev to be trasferred in BLT (4.41)
#define V1731_BUFF_NBLOCKS        0      // x: define # blocks inside buffer (0 <= x <= 10: n=2^x) (4.15)
#define V1731_CHALL_CFG_MEM       1      // 1=sequential access (0=random access)  (4.12)
#define V1731_CHALL_CFG_OVERLAP   0      // 1=enable trigger overlap (4.12)
#define V1731_CHALL_CFG_THR       0      // 1=trig out on under threshold  (4.12)
#define V1731_MAX_BLT_NWORDS      1000   // Max # of 32bit word for BLT
#define V1731_RUN_MODE            0      // 0=register-controlled (4.18)
#define V1731_VME_BERR_ENABLE     0      // 1=enable bus error (4.35 --> does not work with our SBS driver)

//-- Trigger parameters -------------------------------------------------------------------------------
#define V1731_TRIG_MODE           3      // 0=ext/soft off, 1=ext, 2=soft, 3=both (4.21)
#define V1731_TRIG_AUTO_MASK      0xFF   // Define channel source mask for autotrigger (4.21)
#define V1731_TRIG_AUTO_NCH       2      // Define min # of channels over threshold for trig (4.21)

#define V1731_TRIG_OUT_MODE       0      // 0=off, 1=ext, 2=soft, 3=both (output from panel, 4.22)
#define V1731_TRIG_OUT_AUTO_MASK  0xFF   // Define channel source mask for autotrigger (output from panel) (4.21)

#define V1731_TRIG_OVERLAP_ACCEPT 0      // 0=reject overlapping events  (4.18)

//-- Pedestal parameters ------------------------------------------------------------------------------
#define V1731_NEG_SIG_PEDESTAL    0xFA   // Desired pedestal for negative signals
#define V1731_BIP_SIG_PEDESTAL    0x80   // Desired pedestal for bipolar signals
#define V1731_POS_SIG_PEDESTAL    0x05   // Desired pedestal for positive signals
#define V1731_NSAMP_PEDESTAL      200    // # of samplings for pedestal calculation
#define V1731_NSIGMA_PEDESTAL     0.2    // # of sigma for convergence of pedestal setting procedure

//-- Sampling parameters ------------------------------------------------------------------------------
#define V1731_POST_TRIG_PERCENT   90     // Fraction of post trigger samplings (%)
#define V1731_TOT_TIME_WINDOW     100E-6  // Total time window for samplings (s)



//******************************************************************************************************************
//******************************************************************************************************************
//***************************** CAEN V1720 DEFINITIONS *************************************************************
//******************************************************************************************************************
//******************************************************************************************************************
#define V1720_CH_ENABLE_MASK      0xFF   // 0xFF = all channels enabled
#define V1720_CH_CONFIG_MASK      0xEFB5 // All channels (4.12)
#define V1720_CHALL_CFG_ZEROSUPP  0      // 0=disabled; 2=zero length encod (ZLE); 3=full supp based on ampl (4.12)
#define V1720_CHALL_CFG_PACK25    0      // 0=disabled; 1=enabled (4.12)
// ----------------------------------------------------------------------------------------------------
// Definition of quantities for each channels: they must stay on 1 line, otherwise error! (4.3) -------
// ----------------------------------------   Ch1    Ch2    Ch3    Ch4    Ch5    Ch6    Ch7    Ch8   --
static const int V1720_THRESHOLD_ZS[]   ={  0xf87, 0xf87, 0xf87, 0xf87, 0xf87, 0xf87, 0xf87, 0xf87 };   //Thesholds for zero suppression(4.3)
static const int V1720_N_LOOK_BACK_ZS[] ={     10,    10,    10,    10,    10,    10,    10,    10 };   //Backward samples (before start thr)
static const int V1720_N_LOOK_FORW_ZS[] ={     10,    10,    10,    10,    10,    10,    10,    10 };   //Forward  samples (after end thr)
static const int V1720_DAC_VOLT_OFFSET[]={ 0x8000,0x8000,0x8000,0x8000,0x8000,0x8000,0x8000,0x8000 };   //DAC values 16 bit(-0.5V to 0.5V) (4.10) //Not Used
static const int V1720_SIGNAL_POLARITY[]={     -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1 };   //Signal polarity (pos, neg or 0=bipolar )
static const int V1720_CUSTOM_PEDESTAL[]={    0x0,   0x0,   0x0,   0x0,   0x0,   0x0,   0x0,   0x0 };   //Signal polarity (pos, neg or 0=bipolar ) //Not Used
static const int V1720_AUTOTRIG_THR[]   ={  0xf20, 0xf20, 0xf20, 0xf20, 0xf20, 0xf20, 0xf20, 0xf20 };   //Thesholds for auto trigger(4.3) //Normally Not Used
static const int V1720_AUTOTRIG_N_THR[] ={  0x001, 0x001, 0x001, 0x001, 0x001, 0x001, 0x001, 0x001 };   //# of sampl over thr for auto trigger(4.3) //Normally Not Used

// -- General parameters ------------------------------------------------------------------------------
#define V1720_BLT_NEV             0      // # of complete ev to be trasferred in BLT (4.41)
#define V1720_BUFF_NBLOCKS        0      // x: define # blocks inside buffer (0 <= x <= 10: n=2^x) (4.15)
#define V1720_CHALL_CFG_MEM       1      // 1=sequential access (0=random access)  (4.12)
#define V1720_CHALL_CFG_OVERLAP   0      // 1=enable trigger overlap (4.12)
#define V1720_CHALL_CFG_THR       0      // 1=trig out on under threshold  (4.12)
#define V1720_MAX_BLT_NWORDS      250    // Max # of 32bit word for BLT
#define V1720_RUN_MODE            0      // 0=register-controlled (4.18)
#define V1720_VME_BERR_ENABLE     0      // 1=enable bus error (4.35 --> does not work with our SBS driver)

//-- Trigger parameters -------------------------------------------------------------------------------
#define V1720_TRIG_MODE           3      // 0=ext/soft off, 1=ext, 2=soft, 3=both (4.21)
#define V1720_TRIG_AUTO_MASK      0xFF   // Define channel source mask for autotrigger (4.21)
#define V1720_TRIG_AUTO_NCH       2      // Define min # of channels over threshold for trig (4.21)

#define V1720_TRIG_OUT_MODE       0      // 0=off, 1=ext, 2=soft, 3=both (output from panel, 4.22)
#define V1720_TRIG_OUT_AUTO_MASK  0xFF   // Define channel source mask for autotrigger (output from panel) (4.21)

#define V1720_TRIG_OVERLAP_ACCEPT 0      // 0=reject overlapping events  (4.18)

//-- Pedestal parameters ------------------------------------------------------------------------------
#define V1720_NEG_SIG_PEDESTAL    0xFA0  // Desired pedestal for negative signals
#define V1720_BIP_SIG_PEDESTAL    0x800  // Desired pedestal for bipolar signals
#define V1720_POS_SIG_PEDESTAL    0x150  // Desired pedestal for positive signals
#define V1720_NSAMP_PEDESTAL      200    // # of samplings for pedestal calculation
#define V1720_NSIGMA_PEDESTAL     1      // # of sigma for convergence of pedestal setting procedure

//-- Sampling parameters ------------------------------------------------------------------------------
#define V1720_POST_TRIG_PERCENT   90     // Fraction of post trigger samplings (%)
#define V1720_TOT_TIME_WINDOW     100E-6  // Total time window for samplings (s)



void sig_func(int pippo);
char *GetDateStr();

#endif
