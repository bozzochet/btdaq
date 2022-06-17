#include <stdio.h>
#include <iostream>
#include <stdlib.h>
#include <sys/time.h>
#include <sys/stat.h>
#include <math.h>
#include <utility>      // std::pair, std::make_pair
#include <numeric>      // std::accumulate
//----------------ROOT includes---------------------------
#include "TCanvas.h"
#include "TPad.h"
#include "TNtuple.h"
#include "TH2F.h"
#include "TH1F.h"
#include "TGraph.h"
#include "TLine.h"
#include "TText.h"
#include "TLatex.h"
#include "TStyle.h"
#include "TROOT.h"
#include "TString.h"
#include "TApplication.h"
#include "TFile.h"

void LinesVas(int nva=16, int nch_for_va=64);
int InitStyle();

void OpenAMSL0VladimirFile(TString filename, std::vector<TH1F*>& histos);
void ReOrderVladimir(std::vector<unsigned char>& data,  std::vector<unsigned short>& data_ord);

void OpenGigiFile(TString filename, std::vector<TH1F*>& histos);
std::pair<TH1F*, TH1F*> GigiGraphToHisto(const char* name, const char* filename, bool dummy=false);

void CreateAMSFlightHistos(std::vector<TH1F*>& histos);

std::pair<TH1F*, TH1F*> VectorToHisto(std::vector<double> values, const char* name, const char* filename, int nch=640, int nspread=4096);
int ReadFile(void *ptr, size_t size, size_t nitems, FILE *stream);

void Summary(std::vector<TH1F*> histos, const char* filename, const char* nameout, int type=0);
void Comparison(std::vector<TH1F*> histos[3], const char* nameout);

//--------------- Xudong's stuff -------------------------

#ifndef _BYTESWAP_H
#define _BYTESWAP_H

//#warning "byteswap.h is an unportable GNU extension! Don't use!"

static inline unsigned short bswap_16(unsigned short x) {
  return (x>>8) | (x<<8);
}

static inline unsigned int bswap_32(unsigned int x) {
  return (bswap_16(x&0xffff)<<16) | (bswap_16(x>>16));
}

static inline unsigned long long bswap_64(unsigned long long x) {
  return (((unsigned long long)bswap_32(x&0xffffffffull))<<32) |
    (bswap_32(x>>32));
}

#endif

int debug_output=1;
static uint64_t _utime = 0;

#include "aallib/sspdefs.h"
#include "aallib/abi.h"
#include "aallib/aalAlloc.h"
#include "aallib/aallib.c"
#include "aallib/abiFile.c"
#include "aallib/abiConstruct.c"
//#include "aallib/abiPrint.c"
#include "decode_funcs.h"
#include "node_address.h"
#include "node_address.c"

static int usbbox_mode = USB_LINF_V1;
static char *test_info = NULL;

int16 calculate_CRC16(int16 *dat, int16 len);

static const uint32_t huffman_codes[256] = {
  0x00030002, 0x00140005, 0x00150005, 0x00240006, 0x00410007, 0x00420007, 0x00250006, 0x00260006,
  0x00270006, 0x00430007, 0x005e0008, 0x005f0008, 0x00600008, 0x00610008, 0x00620008, 0x00630008,
  0x00640008, 0x00440007, 0x00650008, 0x00660008, 0x00670008, 0x00680008, 0x001a0009, 0x001b0009,
  0x00690008, 0x006a0008, 0x001c0009, 0x001d0009, 0x001e0009, 0x001f0009, 0x00200009, 0x00210009,
  0x006b0008, 0x006c0008, 0x00220009, 0x00230009, 0x00240009, 0x00250009, 0x00260009, 0x00270009,
  0x00280009, 0x00290009, 0x002a0009, 0x002b0009, 0x002c0009, 0x002d0009, 0x002e0009, 0x002f0009,
  0x00300009, 0x00450007, 0x006d0008, 0x006e0008, 0x00310009, 0x00320009, 0x00330009, 0x00340009,
  0x00350009, 0x00360009, 0x00370009, 0x00380009, 0x00390009, 0x003a0009, 0x003b0009, 0x003c0009,
  0x006f0008, 0x003d0009, 0x003e0009, 0x003f0009, 0x00400009, 0x00410009, 0x00420009, 0x00430009,
  0x00440009, 0x00450009, 0x00460009, 0x00470009, 0x00480009, 0x00490009, 0x004a0009, 0x004b0009,
  0x004c0009, 0x004d0009, 0x004e0009, 0x004f0009, 0x00500009, 0x00700008, 0x00710008, 0x00720008,
  0x00510009, 0x00730008, 0x00520009, 0x00460007, 0x00740008, 0x00530009, 0x00540009, 0x00550009,
  0x00560009, 0x00750008, 0x00570009, 0x00580009, 0x00590009, 0x005a0009, 0x005b0009, 0x005c0009,
  0x005d0009, 0x005e0009, 0x005f0009, 0x00600009, 0x000a000a, 0x000b000a, 0x000c000a, 0x00610009,
  0x00620009, 0x00630009, 0x00640009, 0x000d000a, 0x000e000a, 0x00650009, 0x000f000a, 0x0010000a,
  0x0011000a, 0x00660009, 0x0012000a, 0x0013000a, 0x0014000a, 0x0015000a, 0x00670009, 0x00760008,
  0x00160005, 0x00680009, 0x00770008, 0x00780008, 0x00790008, 0x00690009, 0x006a0009, 0x006b0009,
  0x006c0009, 0x006d0009, 0x006e0009, 0x006f0009, 0x00700009, 0x00710009, 0x0016000a, 0x0017000a,
  0x00720009, 0x00730009, 0x0018000a, 0x0019000a, 0x001a000a, 0x00740009, 0x00750009, 0x00760009,
  0x001b000a, 0x001c000a, 0x001d000a, 0x001e000a, 0x001f000a, 0x0020000a, 0x0021000a, 0x00770009,
  0x007a0008, 0x007b0008, 0x00780009, 0x00790009, 0x007c0008, 0x007a0009, 0x007b0009, 0x007c0009,
  0x007d0009, 0x007d0008, 0x007e0009, 0x007f0009, 0x00800009, 0x00810009, 0x00820009, 0x00830009,
  0x00840009, 0x007e0008, 0x00850009, 0x00860009, 0x00870009, 0x007f0008, 0x00880009, 0x00890009,
  0x0022000a, 0x00800008, 0x0023000a, 0x0024000a, 0x0025000a, 0x0026000a, 0x008a0009, 0x008b0009,
  0x008c0009, 0x008d0009, 0x0027000a, 0x0028000a, 0x0029000a, 0x008e0009, 0x002a000a, 0x002b000a,
  0x002c000a, 0x008f0009, 0x002d000a, 0x002e000a, 0x002f000a, 0x0030000a, 0x0031000a, 0x0032000a,
  0x00900009, 0x00910009, 0x00920009, 0x0033000a, 0x00930009, 0x00940009, 0x00950009, 0x00960009,
  0x00970009, 0x00980009, 0x00990009, 0x009a0009, 0x009b0009, 0x009c0009, 0x009d0009, 0x009e0009,
  0x00470007, 0x009f0009, 0x00a00009, 0x00a10009, 0x00a20009, 0x00a30009, 0x00a40009, 0x00a50009,
  0x00a60009, 0x00a70009, 0x00a80009, 0x00a90009, 0x00aa0009, 0x00ab0009, 0x00ac0009, 0x00ad0009,
  0x00ae0009, 0x00af0009, 0x00b00009, 0x00b10009, 0x00b20009, 0x00b30009, 0x00b40009, 0x00b50009,
  0x00b60009, 0x00b70009, 0x00b80009, 0x00b90009, 0x00ba0009, 0x00bb0009, 0x00810008, 0x00170005,
};

// Huffman decoding arrays
static uint16_t huffman_first_code[16];
static uint16_t huffman_decode_table[16][256];
static uint16_t huffman_restore[0x8000];
static int huffman_swap = 0;
static int huffman_inited = 0;

static uint16_t __huffman_bit(uint16_t *data, int bit);
static void __huffman_decode_init(void);
static uint16_t __huffman_decode(uint16_t *istream, uint16_t length, uint16_t **dest );

enum {
  JINJ = 0,
  JLV1,
  TRD,
  TRK,
  RICH,
  ECAL,
  TOF,
  NSD,
};

static const int sl_idx[24] = {
  TRK,  TRK,  TRD,  TRK,  TOF, TOF, TOF, TOF, TRD, TRK, RICH, RICH,
  ECAL, ECAL, JLV1, JLV1, TRK, TRK, TOF, TOF, TOF, TOF, TRK,  TRK,
};

static int evt_sizes[NSD] = {0, 0, 0, 0, 0, 0, 0};
static int evt_cnt = 0;
static int nerrs = 0;
static int jinj_errs[4][3] = {{0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}};

static uint32_t old_evt_id = 0;             // Old Run ID
static uint32_t old_evt_no = 0xFFFFFFFF;    // Old Event Number
static int trig_time_min = 0x10000;         // Minimum time from previous trigger
static int trig_time_max = 0;               // Maximum time from previous trigger
static int trig_time_sum = 0;               // Sum of the time from previous trigger
static uint32_t old_time = 0;               // Old event time in sec.
static uint64_t old_evt_time = 0;           // Remember the last event time as time to write to DB

static uint64_t evt_block_size = 0;         // Event block size
static uint64_t evt_comp_size  = 0;         // Compressed event size from JINJs
static int64_t time_diff = 0;               // Time difference
static int ntime_data = 0;                  // Number of time data

typedef void subd_evt_func(const int id, uint16_t *data, const int len);

/*
  static subd_evt_func *funcs[24] = {
  decode_jinft_event, decode_jinft_event,
  decode_jinfu_event, decode_jinft_event,
  decode_sdr_event,   decode_sdr_event,
  decode_sdr_event,   decode_sdr_event,
  decode_jinfu_event, decode_jinft_event,
  decode_jinfr_event, decode_jinfr_event,
  decode_jinfe_event, decode_jinfe_event,
  NULL,               NULL,
  decode_jinft_event, decode_jinft_event,
  decode_sdr_event,   decode_sdr_event,
  decode_sdr_event,   decode_sdr_event,
  decode_jinft_event, decode_jinft_event,
  };
*/

static subd_evt_func *funcs[24] = {
  NULL, NULL,
  NULL, NULL,
  NULL, NULL,
  NULL, NULL,
  NULL, NULL,
  NULL, NULL,
  NULL, NULL,
  NULL,               NULL,
  NULL, NULL,
  NULL, NULL,
  NULL, NULL,
  NULL, NULL,
};


#define JINJ_BINS   40
static int jinj_bins[JINJ_BINS] = {0};
static const int jinj_bin_step = 100;
static const int jinj_bin_min  = 1200;

#define TRK_BINS   40
static int trk_bins[TRK_BINS] = {0};
static const int trk_bin_step = 50;
static const int trk_bin_min  = 500;

static int old_fileno = -1;

// Define error code for events
char *event_errcode[16] = {
  NULL,               // 0x0
  "err_BC_1",         // 0x1
  "err_room_1",       // 0x2
  "err_data",         // 0x3
  "err_mismatch_1",   // 0x4
  "err_CRC_1",        // 0x5
  "err_timeout",      // 0x6
  "err_NEXT",         // 0x7
  "err_BC_2",         // 0x8
  "err_room_2",       // 0x9
  "err_room_3",       // 0xA
  "err_mismatch_2",   // 0xB
  "err_CRC_2",        // 0xC
  NULL,               // 0xD
  NULL,               // 0xE
  NULL,               // 0xF
};

/*
  static daq_func_t *tracker_funcs[3] = {
  decode_tracker_cal, decode_tracker_cfg, decode_tracker_hkd,
  };
  static daq_func_t *jlv1_funcs[3]    = {
  decode_jlv1_cal,    decode_jlv1_cfg,    decode_jlv1_hkd,
  };
  static daq_func_t *rich_funcs[3]    = {
  decode_rich_cal,    decode_rich_cfg,    decode_rich_hkd,
  };
  static daq_func_t *ecal_funcs[3]    = {
  decode_ecal_cal,    decode_ecal_cfg,    decode_ecal_hkd,
  };
  static daq_func_t *trd_funcs[3]     = {
  decode_trd_cal,     decode_trd_cfg,     decode_trd_hkd,
  };
  static daq_func_t *tof_funcs[3]     = {
  decode_tof_cal,     decode_tof_cfg,     decode_tof_hkd,
  };
*/
static daq_func_t *tracker_funcs[3] = {
  NULL, NULL, NULL
};
static daq_func_t *jlv1_funcs[3]    = {
  NULL, NULL, NULL
};
static daq_func_t *rich_funcs[3]    = {
  NULL, NULL, NULL
};
static daq_func_t *ecal_funcs[3]    = {
  NULL, NULL, NULL
};
static daq_func_t *trd_funcs[3]     = {
  NULL, NULL, NULL
};
static daq_func_t *tof_funcs[3]     = {
  NULL, NULL, NULL
};
static uint32_t old_daq_id = 0;

static void __decode_daq_data(AMSBlock *block, uint64_t utime);

float ds_limits[12][9][64][4] = {{{{NAN}}}};

const int ds_nsens[12][9] = {
  // JPD-A
  {10, 16, 40, 48, 28, 31, 2,  12, 1},
  // JPD-B
  {10, 16, 40, 48, 28, 31, 2,  12, 1},
  // M-A
  {10, 16, 45, 14, 0,  2,  11, 0,  1},
  // M-B
  {10, 16, 45, 14, 0,  2,  11, 0,  1},
  // CCEB-A
  {0,  0,  0,  0,  0,  0,  0,  0,  1},
  // CCEB-B
  {0,  0,  0,  0,  0,  0,  0,  0,  1},
  // PDS-A
  {0,  0,  0,  0,  0,  0,  0,  0,  1},
  // PDS-B
  {0,  0,  0,  0,  0,  0,  0,  0,  1},
  // UG-A
  {30, 30, 30, 22, 30, 30, 30, 25, 1},
  // UG-B
  {31, 21, 30, 38, 30, 21, 31, 25, 1},
  // TTCE-A
  {16, 9,  16, 8,  0,  0,  0,  0,  0},
  // TTCE-B
  {16, 9,  16, 8,  0,  0,  0,  0,  0},
};

static void __decode_gtsn_table(AMSBlock *block, uint64_t utime);
static void __decode_jbux_pointers(AMSBlock *block, uint64_t utime);
static void __decode_utt_sts(AMSBlock *block, uint64_t utime);

static int old_evt = -1, equal_cnt = 0;

static int jmdc_id = -1;
static char *jmdc_chd_tags = NULL;

static state_var_t jmdc_alert = {
  0,
  0,
  NULL,
  "alert",
  NULL,
  "alert",
  NULL,
};

static void __decode_event(AMSBlock *block, uint64_t utime);
void decode_jmdc(AMSBlock *block, uint64_t utime);
void decode_fep(AMSBlock *block, uint64_t utime);

#define NCH     1024
#define NVA     16
#define NVACH   (NCH / NVA)
#define NADC    8

double raw_data[NCH][LEF_RAW_MAX+100];
int nevents = 0;
uint64_t event_time = 0;     // last event time for write DB
int last_node = -1;          // Last node address used

char *usbbox_names[] = {"usbTTTC", "usbTTPC", "usb1553", "usbGSE", "usbFGSE", "usbLINF_v1", NULL};

//----------------------------------------------------------
//  Local variables
//----------------------------------------------------------

static char prename[100] = {""};    // Name prefix
static int file_no = -1;            // MD added
static int last_file = -1;          // Last file number decoded
static double results[4][NCH];

enum __results {
  R_PED = 0,      // Pedestal
  R_SIG,          // Sigma
  R_RSIG,         // Raw sigma
  R_SIGN,         // Signals (???)
};

static int __compare(const void *a, const void *b);
void lef_raw_calib(const int mode, const char *info, const char *fname);
void decode_lef_raw_event(AMSBlock *block, uint64_t utime, const int mode, const char *info);
void decode_l0_upgrade(AMSBlock *block, uint64_t utime, const int mode, const char *info);

char block_time[50] = "\0";
static state_var_t *block_errs = NULL;

static state_var_t blk_nerrs = {
  -1,
  0,
  "BlockErrors,node=NErrors,address=512",
  "nerrors",
  NULL,
  NULL,
  NULL,
};

static bool process_block(void *caller_data, AMSBlock *block);

void OpenAMSL0FEPFile(TString filename, std::vector<TH1F*>& histos);
//-----------------------------------------------------------------------------


//--------------------------------------------------------------------------------------------
//                              Here comes the main...
//-------------------------------------------------------------------------------------------
void plotta_cal(){

  bool amsflight=true;
  bool pox=false;
  bool amsl0vlad=true;
  bool amsl0fep=true;
  
  InitStyle();

  gROOT->SetStyle("StyleWhite");
  //  gStyle->SetPaperSize(27,20);
  
  char nameout[255];
  sprintf(nameout, "Calibrations.pdf");

  const int npoxrootfiles = 4;
  TString poxrootfiles[npoxrootfiles] = {"POX02.root", "POX12_newVA.root", "POX12_VAonly.root", "POX12_completo_50Vbias.root"};

  /*
    const int namsl0vladrootfiles = 3;
    TString amsl0vladrootfiles[namsl0vladrootfiles] = {"LEFP01_noVA.bin", "LEFP03_2.bin", "LEFP03_3.bin"};
  */
  /*
    const int namsl0vladrootfiles = 1;
    TString amsl0vladrootfiles[namsl0vladrootfiles] = {"LEFP03_17.bin"};
  */
  const int namsl0vladrootfiles = 5;
  TString amsl0vladrootfiles[namsl0vladrootfiles] = {"LEFP03_18.bin", "LEFP03_19.bin", "LEFP03_20.bin", "LEFP03_21.bin", "LEFP03_22.bin"};

  const int namsl0feprootfiles = 1;
  TString amsl0feprootfiles[namsl0vladrootfiles] = {"./Data/L0/BLOCKS/USBL0_PG_LEFV2BEAM1/0005/519"};
  
  std::vector<TH1F*> comparison[3];//3 since there're 3 comparisons: pedestal, sigma and sigmawas

  {
    std::vector<TH1F*> histos;
    char nameouttemp[255];
    sprintf(nameouttemp, "%s[", nameout);
    Summary(histos, "", nameouttemp);
  }
  
  if (amsflight) {
    std::vector<TH1F*> histos;
    CreateAMSFlightHistos(histos);
    char nameouttemp[255];
    sprintf(nameouttemp, "%s", nameout);
    Summary(histos, "AMS-flight", nameouttemp);
    for (int jj=0; jj<3; jj++) {//3 since there're 3 comparisons: pedestal, sigma and sigmawas
      comparison[jj].push_back(histos[4+jj]);//4+jj since the first 4 plots are other stuff (pedestal, sigma, sigma raw and average signal)
    }
  }

  if (pox) {
    for (int ii=0; ii<npoxrootfiles; ii++) {
      std::vector<TH1F*> histos;
      OpenGigiFile(poxrootfiles[ii], histos);
      char nameouttemp[255];
      sprintf(nameouttemp, "%s", nameout);
      Summary(histos, poxrootfiles[ii], nameouttemp);
      if (poxrootfiles[ii]=="POX12_completo_50Vbias.root") {
	for (int jj=0; jj<3; jj++) {//3 since there're 3 comparisons: pedestal, sigma and sigmawas
	  comparison[jj].push_back(histos[4+jj]);//4+jj since the first 4 plots are other stuff (pedestal, sigma, sigma raw and average signal)
	}
      }
    }
  }

  if (amsl0vlad) {
    for (int ii=0; ii<namsl0vladrootfiles; ii++) {
      std::vector<TH1F*> histos;
      OpenAMSL0VladimirFile(amsl0vladrootfiles[ii], histos);
      char nameouttemp[255];
      sprintf(nameouttemp, "%s", nameout);
      Summary(histos, amsl0vladrootfiles[ii], nameouttemp, 1);
      if (amsl0vladrootfiles[ii].Contains("LEFP03_")) {
	for (int jj=0; jj<3; jj++) {//3 since there're 3 comparisons: pedestal, sigma and sigmawas
	  comparison[jj].push_back(histos[4+jj]);//4+jj since the first 4 plots are other stuff (pedestal, sigma, sigma raw and average signal)
	}
      }
    }
  }

  if (amsl0fep) {
    for (int ii=0; ii<namsl0feprootfiles; ii++) {
      std::vector<TH1F*> histos;
      OpenAMSL0FEPFile(amsl0feprootfiles[ii], histos);
      /*
      char nameouttemp[255];
      sprintf(nameouttemp, "%s", nameout);
      Summary(histos, amsl0feprootfiles[ii], nameouttemp, 1);
      //      if (amsl0feprootfiles[ii].Contains("LEFP03_")) {
      for (int jj=0; jj<3; jj++) {//3 since there're 3 comparisons: pedestal, sigma and sigmawas
	comparison[jj].push_back(histos[4+jj]);//4+jj since the first 4 plots are other stuff (pedestal, sigma, sigma raw and average signal)
      }
      //      }
      */
    }
  }
  
  {
    char nameouttemp[255];
    sprintf(nameouttemp, "%s)", nameout);
    Comparison(comparison, nameouttemp);
  }
      
  return;
}

int ReadFile(void *ptr, size_t size, size_t nitems, FILE *stream) {
  
  int ret = 0;
  ret = fread(ptr, size, nitems, stream);
  if (feof(stream)) {
    printf("\n");
    printf("End of File \n");
    return -1;
  }
  if (ferror(stream)) {
    printf("Error reading \n");
    return -2;
  }

  return ret;
}

void OpenAMSL0VladimirFile(TString filename, std::vector<TH1F*>& histos){

  FILE* rawfile = fopen(filename.Data(), "r");
  if (rawfile == NULL) {
    printf("Error file %s not found \n", filename.Data());
    exit(2);
  }

  int ret=0;

  std::vector<std::vector<unsigned short>> signals_by_ev;
  std::vector<std::vector<unsigned short>> signals;
  
  int nev=0;
  while (ret>=0) {
    std::vector<unsigned char> data;
    
    unsigned short size=0;
    ret = ReadFile(&size, sizeof(size), 1, rawfile);
    if (ret<=0) break;
    //    printf("%d) %04hx (%d)\n", ret, size, size);
    if (size<=2) break;
    size = size-2;//ha contato la size stessa nella size
    data.resize(2*size);
    
    unsigned short dt=0;
    ret = ReadFile(&dt, sizeof(dt), 1, rawfile);
    if (dt != 0x8013) {
      printf("Wrong DT: %04hx\n", dt);
      break;
    }
    //    printf("%d) %04hx\n", ret, dt);

    unsigned short evtn=0;
    ret = ReadFile(&evtn, sizeof(evtn), 1, rawfile);
    int evtn_int = evtn>>8;
    //    printf("%d) %04hx (%d)\n", ret, evtn, evtn_int);
    
    ret = ReadFile(&data[0], size*sizeof(short), 1, rawfile);
    /*
      for (int ii=0; ii<((int)data.size()); ii++) {
      printf("%d) %02hx\n", ret, data[ii]);
      } 
    */

    std::vector<unsigned short> data_ord;
    ReOrderVladimir(data, data_ord);
    signals_by_ev.push_back(data_ord);
    
    nev++;
    //    printf("We read %d events\n", nev);
  }
  printf("We read %d events\n", nev);

  if (rawfile)
    fclose(rawfile);
  rawfile = NULL;
  
  int nch = ((int)(signals_by_ev[0].size()));
  //  printf("nch = %d\n", nch);
  signals.resize(nch);
  for (int ch = 0; ch < ((int)(signals.size())); ch++) {
    signals[ch].resize(nev);
    for (int ev = 0; ev < ((int)(signals[ch].size())); ev++) {
      signals[ch][ev] = signals_by_ev[ev][ch];
    }
  }
  
  std::vector<double> values[4];
  for (int ii=0; ii<4; ii++) {
    values[ii].resize(signals.size());
  }
  
  for (int ch = 0; ch < ((int)(signals.size())); ch++) {
    //    std::sort(begin(signals[ch]), end(signals[ch]));
    
    values[0][ch] = std::accumulate(begin(signals[ch]), end(signals[ch]), 0.0) /
      static_cast<float>(signals[ch].size());
    //    printf("ped[%d]) %f\n", ch, values[0][ch]);
    
    values[2][ch] =
      std::sqrt(std::accumulate(begin(signals[ch]), end(signals[ch]), 0.0,
				[&](float acc, float curr) {
				  return acc + (curr - values[0][ch]) * (curr - values[0][ch]);
				}) /
		// this is the `real` RMS. Not ok
		//      std::sqrt((std::inner_product(signals[ch].begin(), signals[ch].end(), signals[ch].begin(), 0.0f) - values[0][ch]*values[0][ch]) /
		static_cast<float>(signals[ch].size()));
    //    printf("rsig[%d]) %f\n", ch, values[2][ch]);
    
    // initialize this for later
    values[1][ch] = 0;
    values[3][ch] = 0;
  }
  
  {
    unsigned int NVAS = 16;
    unsigned int NCHAVA = 64;     
    unsigned int lastVA = std::numeric_limits<unsigned int>::max();
    std::vector<float> common_noise(NVAS);
    std::vector<unsigned int> processed_events(NVAS * NCHAVA);
    for (unsigned int iEv = 0; iEv < signals[0].size(); ++iEv) {
      for (unsigned int iCh = 0; iCh < (NVAS * NCHAVA); ++iCh) {
	unsigned int thisVA = iCh / NCHAVA;
	if (thisVA != lastVA) {
	  
	  std::vector<float> sig_mean_sub;
	  for (unsigned int iVACh = 0; iVACh < NCHAVA; ++iVACh) {
	    double sig = signals[thisVA * NCHAVA + iVACh][iEv] - values[0][thisVA * NCHAVA + iVACh];
	    double rawnoise = values[2][thisVA * NCHAVA + iVACh];
	    double sig_to_rawnoise = sig/rawnoise;
	    if (fabs(sig_to_rawnoise)<3) {
	      sig_mean_sub.push_back(sig);
	    }
	    /* else { */
	    /*   if (iEv==43) printf("%d) %f\n", thisVA * NCHAVA + iVACh, sig_to_rawnoise); */
	    /* } */
	  }
	  
	  // get the median
	  std::sort(begin(sig_mean_sub), end(sig_mean_sub));
	  //	  common_noise[thisVA] = 0.5 * (sig_mean_sub[(NCHAVA / 2) - 1] + sig_mean_sub[NCHAVA / 2]);
	  common_noise[thisVA] = 0.5 * (sig_mean_sub[(((int)(sig_mean_sub.size())) / 2) - 1] + sig_mean_sub[((int)(sig_mean_sub.size())) / 2]);
	  //	  if (iEv==0) printf("VA=%d) CN for sigma=%f (%lu/%d)\n", thisVA, common_noise[thisVA], sig_mean_sub.size(), NCHAVA);
	}
	
	if (std::fabs(common_noise[thisVA]) > 10) {//not used for the sigma evaluation
	  continue;
	}

	++processed_events[iCh];
	
	values[1][iCh] += (signals[iCh][iEv] - values[0][iCh] - common_noise[thisVA]) *
	  (signals[iCh][iEv] - values[0][iCh] - common_noise[thisVA]);
	
	lastVA = thisVA;
      }
    }
    for (unsigned int iCh = 0; iCh < NVAS * NCHAVA; ++iCh) {
      values[1][iCh] = std::sqrt(values[1][iCh] / static_cast<float>(processed_events[iCh]));
      //      printf("sig[%d]) %f\n", iCh, values[1][iCh]);
    }
    
    for (unsigned int iEv = 0; iEv < signals[0].size(); ++iEv) {
      for (unsigned int iCh = 0; iCh < (NVAS * NCHAVA); ++iCh) {
	unsigned int thisVA = iCh / NCHAVA;
	if (thisVA != lastVA) {
	  
	  std::vector<float> sig_mean_sub;
	  for (unsigned int iVACh = 0; iVACh < NCHAVA; ++iVACh) {
	    double sig = signals[thisVA * NCHAVA + iVACh][iEv] - values[0][thisVA * NCHAVA + iVACh];
	    double noise = values[1][thisVA * NCHAVA + iVACh];
	    double sig_to_noise = sig/noise;
	    if (fabs(sig_to_noise)<3.5) {
	      sig_mean_sub.push_back(sig);
	      //	      printf("%f\n", sig_mean_sub[iVACh]);
	    }
	  }
	  
	  // get the median
	  std::sort(begin(sig_mean_sub), end(sig_mean_sub));
	  //	  common_noise[thisVA] = 0.5 * (sig_mean_sub[(NCHAVA / 2) - 1] + sig_mean_sub[NCHAVA / 2]);
	  common_noise[thisVA] = 0.5 * (sig_mean_sub[(((int)(sig_mean_sub.size())) / 2) - 1] + sig_mean_sub[((int)(sig_mean_sub.size())) / 2]);
	  //	  if (iEv==0) printf("VA=%d) CN for average=%f (%lu/%d)\n", thisVA, common_noise[thisVA], sig_mean_sub.size(), NCHAVA);
	  /* if (fabs(common_noise[thisVA])>100.0){ */
	  /*   for (int ii=0; ii<((int)(sig_mean_sub.size())); ii++) { */
	  /*     printf("%d) %f (%f)\n", ii, sig_mean_sub[ii], common_noise[thisVA]); */
	  /*   } */
	  /* } */
	}

	double sig = (signals[iCh][iEv] - values[0][iCh] - common_noise[thisVA]);
	double noise = values[1][iCh];
	double sig_to_noise = sig/noise;
	if (sig_to_noise>3.5) {
	  values[3][iCh] += sig;
	}
	/* if (iCh==959 && fabs(sig_to_noise)>100.0) */
	/*   printf("%d %d -> %f (%f) [%hu %f %f %f]\n", iEv, iCh, sig_to_noise, values[3][iCh], signals[iCh][iEv], values[0][iCh], common_noise[thisVA], values[1][iCh]); */
		
	lastVA = thisVA;
      }
    }
    for (unsigned int iCh = 0; iCh < NVAS * NCHAVA; ++iCh) {
      values[3][iCh] = (values[3][iCh] / static_cast<float>(signals[0].size()));
      //      printf("ave_sig[%d]) %f\n", iCh, values[3][iCh]);	    
    }
  }
  /*
    {
    unsigned int NVAS = 16;
    unsigned int NCHAVA = 64;     
    for (unsigned int ch = 0; ch < NVAS * NCHAVA; ch++) {
    printf("%04d)", ch);
    printf("\t ped=%f", values[0][ch]);
    printf("\t rsig=%f", values[2][ch]);
    printf("\t sig=%f", values[1][ch]);
    printf("\t ave_sig=%f\n", values[3][ch]);
    }
    }
  */
  
  const char* names[4] = {"Pedestals", "Sigma", "RawSigma", "AverageSignal"};
  
  std::vector<TH1F*> histos2;
  
  for (int ii=0; ii<4; ii++) {
    std::pair<TH1F*, TH1F*> coppia = VectorToHisto(values[ii], names[ii], filename.Data(), nch);
    TH1F* h = coppia.first;
    TH1F* h2 = coppia.second;
    printf("%s %s %p %p\n", "AMS-L0", names[ii], h, h2);
    /*
      h->Dump();
      for (int bb=0; bb<=h->GetNbinsX()+1; bb++){
      printf("%d) %f\n", bb, h->GetBinContent(bb));
      }
    */
    histos.push_back(h);
    histos2.push_back(h2);
  }

  for (int ii=0; ii<3; ii++) {//not including the spread on the average signal
    histos.push_back(histos2.at(ii));
  }
    
  return;
}

void ReOrderVladimir(std::vector<unsigned char>& data,  std::vector<unsigned short>& data_ord){

  std::vector<unsigned short> data_ord_tmp;
  data_ord_tmp.resize(((int)((8.0/14.0)*((int)(data.size())))));
  
  data_ord.resize(((int)((8.0/14.0)*((int)(data.size())))));
  //  printf("**** data.size=%d, nch = %d, data_ord.size=%d\n", ((int)(data.size())), ((int)((8.0/14.0)*((int)(data.size())))), ((int)(data_ord.size())));
  
  int ch_in_2va = -1;
  
  for (int byte=0; byte<((int)data.size()); byte+=14) {
    ch_in_2va++;
    for (int bit_nec=0; bit_nec<14; bit_nec++) {
      //      int bit = bit_nec%2?(bit_nec-1):(bit_nec+1); //endianess correction, not needed
      int bit = bit_nec;
      //      printf("byte %dth) 0x%02hx\n", byte+bit_nec+1, data[byte+bit_nec]);
      for (int adc=0; adc<8; adc++){
	int ch = adc*128+ch_in_2va;
	data_ord_tmp[ch] |= ((data[byte+bit_nec] & (1<<adc))>>adc)<<bit;
	//	printf("%d %d %d) 0x%02hx\n", adc, ch_in_2va, ch, data_ord[ch]);
      }
    }
  }

  for (int ch=0; ch<((int)data_ord_tmp.size()); ch++) {
    int va = ch/64;
    int ch_in_va = ch%64;
    //    printf("%d -> (%d,%d)\n", ch, va, ch_in_va);
    //    int new_ch = (16-va-1)*64 + (64-ch_in_va-1);//MD: I undersootd like this, but VK did differently (31 Mar 2022)
    int new_ch = (16-va-1)*64 + ch_in_va;
    //    printf("%d --> %d\n", ch, new_ch);
    data_ord[new_ch] = data_ord_tmp[ch];
  }
  
  /*
    for (int ch=0; ch<((int)data_ord.size()); ch++) {
    printf("%d) %02hx (%d)\n", ch, data_ord[ch], data_ord[ch]);
    }
  */
    
  return;
}

void OpenGigiFile(TString filename, std::vector<TH1F*>& histos){

  std::vector<TH1F*> histos2;
  
  const char* names[4] = {"Pedestals", "Sigma", "RawSigma", "AverageSignal"};
  
  for (int ii=0; ii<3; ii++) {
    std::pair<TH1F*, TH1F*> coppia = GigiGraphToHisto(names[ii], filename.Data());
    TH1F* h = coppia.first;
    TH1F* h2 = coppia.second;
    printf("%s %s %p %p\n", filename.Data(), names[ii], h, h2);
    histos.push_back(h);
    histos2.push_back(h2);
  }
  {//dummy
    int ii=3;
    std::pair<TH1F*, TH1F*> coppia = GigiGraphToHisto(names[ii], filename.Data(), true);
    TH1F* h = coppia.first;
    TH1F* h2 = coppia.second;
    //    printf("%s %s %p %p\n", filename.Data(), names[ii], h, h2);
    histos.push_back(h);
    histos2.push_back(h2);
  }
  
  for (int ii=0; ii<3; ii++) {
    histos.push_back(histos2.at(ii));
  }
  
  return;
}

std::pair<TH1F*, TH1F*> GigiGraphToHisto(const char* name, const char* filename, bool dummy){

  TH1F* h = new TH1F(Form("%s_%s", name, filename), Form("%s_%s", name, filename), 640, 0, 640);
  TH1F* h2 = new TH1F(Form("%s_spread_%s", name, filename), Form("%s_spread_%s", name, filename), 409600, 0, 4096);

  if (!dummy) {
    TFile* f = new TFile(filename);
    TGraph* graph = (TGraph*)(f->Get(name));
    //  printf("%p\n", graph);
    //  graph->Dump();
    
    auto nPoints = graph->GetN();
    for(int i=0; i < nPoints; ++i) {
      double x,y;
      graph->GetPoint(i, x, y);
      h->Fill(x, y);
      h2->Fill(y);
    }
    
    f->Close();
  }

  std::pair<TH1F*, TH1F*> coppia = std::make_pair(h, h2);
  
  return coppia;
}

void CreateAMSFlightHistos(std::vector<TH1F*>& histos){

  // HwID 010, TkID +109: JLayer 2, Slot 9
  // extracted from CalDB_1646913042.root
  
  double pedestals[640] = {584.625000, 527.875000, 549.875000, 531.500000, 585.125000, 543.250000, 572.875000, 497.875000, 566.250000, 555.375000, 565.875000, 601.500000, 557.875000, 611.000000, 569.250000, 610.500000, 658.500000, 571.500000, 681.750000, 650.500000, 640.250000, 602.125000, 606.875000, 700.500000, 668.625000, 684.125000, 620.000000, 621.000000, 693.125000, 669.875000, 570.000000, 611.125000, 630.875000, 638.375000, 628.875000, 617.125000, 617.500000, 611.875000, 554.625000, 581.125000, 601.250000, 562.875000, 569.625000, 599.125000, 572.125000, 572.250000, 548.125000, 608.625000, 642.750000, 554.375000, 616.250000, 546.125000, 596.250000, 649.625000, 550.875000, 621.750000, 566.250000, 650.625000, 561.625000, 603.375000, 608.375000, 593.375000, 570.875000, 531.375000, 356.875000, 315.500000, 346.000000, 347.000000, 384.875000, 337.250000, 338.625000, 319.000000, 384.625000, 426.625000, 370.750000, 407.250000, 406.375000, 370.000000, 401.250000, 458.750000, 481.875000, 392.625000, 434.000000, 459.500000, 410.875000, 433.750000, 439.125000, 452.750000, 457.500000, 541.875000, 449.250000, 515.375000, 477.125000, 486.375000, 535.125000, 516.375000, 597.625000, 525.750000, 601.375000, 582.125000, 603.500000, 614.750000, 578.625000, 620.500000, 647.125000, 602.625000, 613.625000, 579.125000, 617.750000, 502.000000, 608.875000, 594.000000, 587.000000, 527.500000, 536.750000, 544.125000, 581.750000, 560.000000, 599.875000, 541.500000, 555.125000, 485.250000, 544.250000, 570.000000, 554.750000, 550.500000, 572.750000, 576.875000, 482.500000, 418.375000, 435.625000, 444.250000, 431.125000, 395.500000, 459.500000, 445.625000, 439.250000, 476.500000, 421.375000, 445.750000, 377.125000, 460.375000, 456.625000, 484.250000, 459.375000, 404.875000, 422.125000, 458.125000, 468.000000, 495.000000, 485.750000, 508.125000, 534.375000, 529.625000, 483.250000, 495.875000, 521.875000, 459.250000, 555.625000, 476.250000, 548.500000, 542.250000, 503.500000, 500.625000, 473.000000, 422.375000, 539.250000, 515.625000, 551.125000, 477.000000, 490.875000, 519.375000, 511.375000, 544.625000, 516.000000, 486.750000, 522.000000, 486.125000, 479.375000, 498.875000, 491.250000, 495.625000, 495.375000, 476.625000, 476.875000, 457.625000, 429.375000, 448.750000, 523.000000, 470.750000, 452.000000, 476.000000, 385.000000, 353.500000, 391.750000, 378.000000, 318.750000, 413.875000, 356.750000, 412.125000, 451.000000, 401.375000, 414.625000, 390.875000, 402.375000, 431.375000, 415.125000, 498.750000, 452.875000, 342.750000, 443.500000, 370.750000, 437.000000, 453.125000, 499.875000, 449.625000, 434.875000, 430.000000, 484.375000, 420.000000, 434.750000, 457.250000, 456.500000, 533.750000, 508.875000, 477.500000, 477.750000, 415.250000, 494.750000, 471.250000, 520.625000, 499.875000, 548.625000, 467.875000, 510.625000, 489.000000, 537.625000, 452.000000, 505.125000, 524.875000, 632.000000, 541.125000, 471.750000, 542.125000, 511.875000, 552.875000, 532.750000, 581.250000, 572.875000, 552.375000, 510.250000, 538.625000, 484.875000, 426.000000, 496.375000, 535.500000, 370.500000, 348.250000, 344.375000, 346.250000, 353.875000, 317.875000, 334.500000, 347.000000, 320.875000, 325.000000, 351.000000, 319.750000, 363.500000, 377.250000, 340.875000, 376.625000, 404.875000, 356.125000, 417.750000, 385.000000, 408.625000, 357.375000, 385.375000, 416.250000, 423.500000, 412.250000, 447.125000, 389.500000, 353.750000, 350.125000, 426.875000, 405.500000, 452.625000, 398.875000, 409.500000, 425.250000, 405.125000, 481.500000, 455.750000, 475.500000, 462.000000, 515.250000, 475.875000, 515.875000, 506.250000, 472.875000, 472.125000, 470.000000, 535.875000, 535.375000, 513.000000, 498.875000, 486.750000, 512.875000, 486.125000, 527.500000, 482.875000, 539.000000, 545.375000, 492.875000, 507.875000, 453.000000, 475.375000, 525.000000, 367.375000, 368.500000, 313.375000, 338.625000, 331.875000, 307.375000, 330.125000, 368.750000, 337.500000, 380.000000, 346.000000, 413.750000, 439.125000, 425.500000, 427.750000, 451.250000, 467.875000, 394.125000, 426.000000, 437.250000, 339.125000, 406.500000, 435.000000, 465.750000, 505.625000, 510.250000, 432.125000, 447.125000, 424.750000, 383.000000, 378.625000, 393.375000, 432.750000, 378.750000, 354.875000, 374.875000, 351.375000, 432.125000, 415.750000, 418.375000, 424.375000, 383.625000, 386.375000, 419.750000, 438.750000, 461.500000, 456.375000, 406.375000, 426.375000, 428.250000, 382.500000, 426.875000, 450.000000, 433.125000, 418.375000, 440.875000, 428.250000, 479.750000, 425.000000, 426.625000, 446.500000, 414.125000, 379.500000, 417.375000, 364.500000, 385.250000, 394.250000, 363.375000, 336.625000, 332.125000, 370.750000, 398.500000, 364.625000, 361.500000, 335.625000, 352.125000, 398.500000, 383.250000, 425.750000, 301.875000, 366.500000, 390.125000, 350.250000, 447.500000, 346.500000, 307.500000, 295.625000, 333.750000, 294.750000, 322.625000, 386.000000, 354.000000, 389.000000, 353.125000, 290.125000, 409.375000, 400.375000, 379.750000, 362.750000, 351.750000, 424.000000, 458.625000, 411.125000, 431.250000, 404.625000, 443.875000, 355.250000, 429.750000, 424.250000, 405.375000, 422.750000, 433.375000, 432.125000, 412.875000, 523.375000, 411.250000, 398.750000, 405.125000, 413.375000, 416.500000, 420.500000, 373.625000, 409.000000, 400.000000, 421.625000, 460.125000, 461.750000, 354.250000, 301.000000, 271.875000, 244.500000, 336.250000, 322.250000, 312.375000, 300.625000, 326.500000, 302.375000, 290.250000, 313.125000, 245.625000, 246.375000, 298.250000, 324.625000, 322.625000, 340.125000, 353.500000, 326.125000, 358.750000, 352.875000, 339.375000, 354.125000, 341.250000, 334.875000, 343.500000, 418.000000, 376.000000, 322.625000, 309.625000, 347.500000, 396.875000, 318.000000, 323.750000, 342.875000, 341.250000, 368.375000, 379.125000, 388.125000, 400.750000, 375.500000, 311.375000, 403.625000, 391.875000, 353.750000, 405.750000, 415.875000, 420.000000, 419.750000, 418.250000, 381.625000, 440.875000, 429.250000, 429.375000, 408.750000, 437.875000, 410.500000, 420.875000, 380.625000, 400.625000, 378.875000, 365.250000, 387.875000, 406.625000, 200.000000, 268.125000, 254.375000, 233.875000, 230.250000, 229.000000, 262.500000, 303.125000, 231.000000, 267.750000, 232.250000, 247.750000, 290.750000, 279.875000, 282.500000, 318.500000, 299.750000, 347.750000, 313.375000, 357.500000, 340.375000, 339.750000, 316.875000, 313.500000, 321.500000, 295.875000, 277.625000, 336.000000, 366.875000, 389.250000, 303.125000, 297.500000, 309.375000, 326.625000, 360.625000, 367.875000, 365.250000, 356.250000, 403.375000, 394.000000, 500.750000, 476.750000, 431.500000, 432.750000, 466.250000, 459.500000, 553.500000, 546.125000, 487.125000, 489.000000, 544.625000, 502.500000, 519.375000, 463.250000, 539.250000, 542.500000, 504.750000, 557.625000, 599.750000, 531.750000, 497.250000, 572.125000, 520.000000, 542.875000, 353.500000, 439.250000, 387.000000, 411.500000, 468.375000, 413.000000, 446.625000, 452.375000, 464.625000, 460.375000, 394.250000, 399.625000, 443.125000, 411.375000, 385.625000, 476.875000, 434.875000, 437.375000, 438.125000, 345.750000, 464.500000, 439.750000, 418.875000, 477.125000, 457.625000, 453.125000, 486.375000, 374.250000, 445.250000, 425.250000, 477.875000, 517.875000, 458.375000, 468.000000, 443.500000, 415.250000, 467.375000, 432.750000, 464.875000, 433.375000, 414.500000, 464.500000, 442.000000, 410.000000, 419.125000, 425.000000, 395.000000, 442.375000, 422.875000, 461.250000, 398.625000, 413.500000, 415.125000, 484.750000, 450.500000, 507.750000, 502.125000, 474.500000, 485.875000, 476.500000, 478.875000, 493.000000, 485.125000, 512.625000}; 

  double sigmas[640] = {4.250000, 3.375000, 3.000000, 2.875000, 2.750000, 2.875000, 2.750000, 3.000000, 2.875000, 2.875000, 3.000000, 2.750000, 2.875000, 2.875000, 2.750000, 2.750000, 2.750000, 2.750000, 2.625000, 2.625000, 2.875000, 2.875000, 2.625000, 2.750000, 2.875000, 2.750000, 2.625000, 2.625000, 2.750000, 2.875000, 2.750000, 2.750000, 2.750000, 2.750000, 3.000000, 2.750000, 2.625000, 2.750000, 2.750000, 2.750000, 2.875000, 2.750000, 2.750000, 2.750000, 2.750000, 2.625000, 2.750000, 2.875000, 2.875000, 2.625000, 2.875000, 2.625000, 2.750000, 2.875000, 2.875000, 2.750000, 2.750000, 2.750000, 2.750000, 2.875000, 2.875000, 2.750000, 2.750000, 3.000000, 2.875000, 3.000000, 2.750000, 2.875000, 2.750000, 2.750000, 2.750000, 2.500000, 2.750000, 2.625000, 2.625000, 2.750000, 2.750000, 2.625000, 2.750000, 2.625000, 2.875000, 2.750000, 2.750000, 2.750000, 2.750000, 2.750000, 2.750000, 2.750000, 2.625000, 2.750000, 2.750000, 2.750000, 2.875000, 2.625000, 2.625000, 2.750000, 2.750000, 2.625000, 2.750000, 2.750000, 2.750000, 8.125000, 2.625000, 2.750000, 2.750000, 2.750000, 2.875000, 2.750000, 2.750000, 2.750000, 2.500000, 2.625000, 2.750000, 2.625000, 2.875000, 2.750000, 2.750000, 2.750000, 2.750000, 2.750000, 2.750000, 2.750000, 2.750000, 2.625000, 2.750000, 2.750000, 2.750000, 2.750000, 2.750000, 2.750000, 2.625000, 2.625000, 2.750000, 2.875000, 2.750000, 2.750000, 2.750000, 2.750000, 2.875000, 2.875000, 2.750000, 2.750000, 2.625000, 2.625000, 2.625000, 2.750000, 2.625000, 2.625000, 2.625000, 2.625000, 2.750000, 2.625000, 2.750000, 2.625000, 2.625000, 2.750000, 2.875000, 2.500000, 2.750000, 2.625000, 2.625000, 2.750000, 2.625000, 2.625000, 2.750000, 2.750000, 2.625000, 2.750000, 2.750000, 2.625000, 2.625000, 2.750000, 2.625000, 2.500000, 2.750000, 2.750000, 2.625000, 2.750000, 2.750000, 2.750000, 2.750000, 2.750000, 2.625000, 2.750000, 2.625000, 2.625000, 2.750000, 2.875000, 2.625000, 2.750000, 2.750000, 3.125000, 2.875000, 2.750000, 2.750000, 2.750000, 2.625000, 2.750000, 2.750000, 2.750000, 2.625000, 2.625000, 2.625000, 2.750000, 2.750000, 2.625000, 2.500000, 2.500000, 2.625000, 2.750000, 2.625000, 2.750000, 2.750000, 2.625000, 2.750000, 2.750000, 2.750000, 2.625000, 2.625000, 2.625000, 2.750000, 2.500000, 2.500000, 4.500000, 2.750000, 2.750000, 2.625000, 2.750000, 2.625000, 2.750000, 2.625000, 2.500000, 2.625000, 2.625000, 2.750000, 2.625000, 2.625000, 2.750000, 2.750000, 2.750000, 2.625000, 2.750000, 2.750000, 2.750000, 2.625000, 2.625000, 2.625000, 2.625000, 2.625000, 2.750000, 2.750000, 2.750000, 2.625000, 2.750000, 2.625000, 2.875000, 2.750000, 2.875000, 2.750000, 2.625000, 2.750000, 2.750000, 2.625000, 2.875000, 2.625000, 2.625000, 2.750000, 2.500000, 2.500000, 2.625000, 2.750000, 2.625000, 2.625000, 2.625000, 2.750000, 2.625000, 2.625000, 2.625000, 2.625000, 2.625000, 2.625000, 2.625000, 2.625000, 2.625000, 2.750000, 2.625000, 2.625000, 2.750000, 2.750000, 2.625000, 2.625000, 2.625000, 2.500000, 2.750000, 2.625000, 2.625000, 2.625000, 2.750000, 2.750000, 2.625000, 2.750000, 2.750000, 2.625000, 2.625000, 2.750000, 2.625000, 2.750000, 2.750000, 2.625000, 2.750000, 2.625000, 2.625000, 2.750000, 2.625000, 2.625000, 2.750000, 2.750000, 2.625000, 2.750000, 2.750000, 7.250000, 2.500000, 2.500000, 2.500000, 2.500000, 2.625000, 2.625000, 2.625000, 2.500000, 2.500000, 2.625000, 2.500000, 2.500000, 2.500000, 2.500000, 2.375000, 2.625000, 2.625000, 2.625000, 2.625000, 2.500000, 2.625000, 2.625000, 2.500000, 2.500000, 2.500000, 2.500000, 2.625000, 2.625000, 2.625000, 2.500000, 2.625000, 2.500000, 2.625000, 2.500000, 2.500000, 2.625000, 2.500000, 2.500000, 2.500000, 2.625000, 2.500000, 2.500000, 2.625000, 2.500000, 2.500000, 2.500000, 2.500000, 2.625000, 2.500000, 2.625000, 2.500000, 2.375000, 2.625000, 2.500000, 2.500000, 2.625000, 2.625000, 2.625000, 2.500000, 2.625000, 4.125000, 3.250000, 2.625000, 3.000000, 2.750000, 2.750000, 2.750000, 2.625000, 2.750000, 2.625000, 2.750000, 2.625000, 2.625000, 2.625000, 2.625000, 2.500000, 2.750000, 2.625000, 2.625000, 2.625000, 2.750000, 2.500000, 2.625000, 2.625000, 2.625000, 2.625000, 2.500000, 2.625000, 2.625000, 2.750000, 2.500000, 2.500000, 2.500000, 2.500000, 2.500000, 2.625000, 2.625000, 2.750000, 2.500000, 2.625000, 2.500000, 2.625000, 2.500000, 2.750000, 2.625000, 2.625000, 2.625000, 2.500000, 2.500000, 2.625000, 2.500000, 2.625000, 2.625000, 2.750000, 2.625000, 2.625000, 2.625000, 2.625000, 2.625000, 2.500000, 2.500000, 2.625000, 2.625000, 2.625000, 2.500000, 2.750000, 2.625000, 2.875000, 2.750000, 2.750000, 2.750000, 2.625000, 2.750000, 2.625000, 2.625000, 2.750000, 2.750000, 2.750000, 2.750000, 2.750000, 2.625000, 2.625000, 2.625000, 2.625000, 2.625000, 2.625000, 2.625000, 2.625000, 2.625000, 2.625000, 2.500000, 2.625000, 2.625000, 2.625000, 2.625000, 2.500000, 2.500000, 2.625000, 2.500000, 2.375000, 2.500000, 2.625000, 2.625000, 2.500000, 2.500000, 2.500000, 2.500000, 2.625000, 2.750000, 2.625000, 2.625000, 2.625000, 2.750000, 2.625000, 2.750000, 2.500000, 2.500000, 2.625000, 2.625000, 2.625000, 2.750000, 2.625000, 2.625000, 2.750000, 2.750000, 2.625000, 2.625000, 2.750000, 2.625000, 2.625000, 2.875000, 2.625000, 2.625000, 2.750000, 2.625000, 2.625000, 2.750000, 2.625000, 2.625000, 2.625000, 2.625000, 2.750000, 2.625000, 2.500000, 2.750000, 2.625000, 2.500000, 2.750000, 2.625000, 2.625000, 2.750000, 2.500000, 2.625000, 2.500000, 2.625000, 2.625000, 2.625000, 2.750000, 2.625000, 2.750000, 2.625000, 2.750000, 2.625000, 2.750000, 2.625000, 2.625000, 2.625000, 2.625000, 2.625000, 2.500000, 2.750000, 2.625000, 2.625000, 2.750000, 2.625000, 2.750000, 2.625000, 2.625000, 2.625000, 2.750000, 2.625000, 2.625000, 2.750000, 2.750000, 2.625000, 2.875000, 2.625000, 2.750000, 2.750000, 2.625000, 2.625000, 2.625000, 2.625000, 2.750000, 2.625000, 2.750000, 2.625000, 3.250000, 2.625000, 2.500000, 2.500000, 2.625000, 2.500000, 2.500000, 2.500000, 2.625000, 2.625000, 2.625000, 2.625000, 2.750000, 2.625000, 2.625000, 2.750000, 2.625000, 2.625000, 2.625000, 2.625000, 2.500000, 2.625000, 2.500000, 2.750000, 24.250000, 2.750000, 2.625000, 2.500000, 2.375000, 2.625000, 7.000000, 2.625000, 7.125000, 2.750000, 8.625000, 2.750000, 2.625000, 2.625000, 2.500000, 2.625000, 2.625000, 2.625000, 2.625000, 2.625000, 2.625000, 2.625000, 2.625000, 2.500000, 2.625000, 2.500000, 2.625000, 2.750000, 2.625000, 2.625000, 2.625000, 2.625000, 2.625000, 2.750000, 2.625000, 2.625000, 2.875000, 3.250000};

  double sigmaraws[640] = {11.750000, 11.625000, 10.250000, 9.750000, 9.750000, 9.750000, 9.750000, 9.875000, 9.750000, 9.750000, 9.875000, 10.000000, 9.625000, 10.000000, 9.750000, 9.625000, 9.750000, 9.375000, 9.750000, 9.750000, 9.625000, 9.750000, 9.500000, 9.750000, 9.625000, 9.500000, 9.625000, 9.625000, 9.625000, 9.625000, 9.500000, 9.500000, 9.500000, 9.750000, 9.625000, 9.625000, 9.500000, 9.500000, 9.500000, 9.375000, 9.625000, 9.500000, 9.250000, 9.375000, 9.375000, 9.375000, 9.375000, 9.625000, 9.625000, 9.625000, 9.750000, 9.750000, 9.625000, 9.875000, 9.625000, 9.500000, 9.375000, 9.750000, 9.500000, 9.750000, 9.625000, 9.500000, 9.375000, 9.375000, 9.500000, 9.125000, 9.125000, 9.375000, 9.125000, 9.000000, 9.000000, 8.875000, 9.125000, 8.875000, 9.000000, 9.125000, 9.000000, 8.875000, 9.000000, 9.125000, 9.125000, 9.000000, 9.000000, 8.750000, 9.125000, 9.000000, 8.875000, 9.000000, 8.875000, 9.125000, 9.000000, 9.000000, 9.000000, 8.875000, 9.125000, 9.000000, 9.125000, 8.875000, 9.125000, 8.875000, 9.250000, 2.500000, 8.750000, 8.875000, 8.875000, 9.125000, 9.125000, 9.125000, 9.000000, 9.375000, 9.250000, 9.125000, 9.125000, 9.125000, 9.000000, 9.125000, 9.000000, 9.000000, 9.000000, 9.000000, 9.000000, 9.000000, 9.000000, 8.750000, 8.875000, 8.750000, 8.750000, 8.625000, 9.250000, 9.125000, 8.750000, 8.875000, 8.625000, 8.875000, 8.875000, 8.750000, 8.875000, 8.875000, 8.625000, 8.875000, 8.750000, 8.750000, 8.625000, 8.625000, 8.750000, 8.750000, 8.750000, 8.625000, 8.750000, 8.625000, 8.875000, 8.625000, 8.750000, 8.875000, 8.625000, 8.750000, 8.625000, 8.375000, 8.625000, 8.625000, 8.625000, 8.500000, 8.625000, 8.750000, 8.500000, 8.500000, 8.625000, 8.500000, 8.375000, 8.500000, 8.375000, 8.375000, 8.375000, 8.250000, 8.500000, 8.375000, 8.500000, 8.250000, 8.375000, 8.500000, 8.500000, 8.250000, 8.250000, 8.125000, 8.375000, 8.250000, 8.250000, 8.375000, 8.250000, 8.125000, 8.125000, 8.000000, 9.000000, 9.000000, 8.750000, 9.000000, 8.375000, 8.625000, 8.625000, 8.625000, 8.500000, 8.625000, 8.625000, 8.625000, 8.375000, 8.375000, 8.500000, 8.500000, 8.500000, 8.250000, 8.625000, 8.500000, 8.500000, 8.375000, 8.500000, 8.125000, 8.375000, 8.250000, 8.375000, 8.250000, 8.250000, 8.250000, 8.375000, 6.375000, 8.375000, 8.250000, 8.500000, 8.375000, 8.375000, 8.500000, 8.500000, 8.500000, 8.500000, 8.500000, 8.375000, 8.875000, 8.500000, 8.500000, 8.625000, 8.375000, 8.625000, 8.500000, 8.625000, 8.500000, 8.625000, 8.625000, 8.500000, 8.500000, 8.250000, 8.500000, 8.500000, 8.250000, 8.500000, 8.250000, 8.125000, 8.125000, 9.000000, 8.875000, 8.625000, 8.750000, 8.750000, 8.750000, 8.375000, 8.500000, 8.375000, 8.375000, 8.500000, 8.250000, 8.625000, 8.375000, 8.500000, 8.500000, 8.375000, 8.375000, 8.375000, 8.500000, 8.625000, 8.375000, 8.250000, 8.625000, 8.125000, 8.375000, 8.250000, 8.125000, 8.250000, 8.375000, 8.000000, 8.000000, 8.125000, 8.250000, 8.250000, 8.125000, 8.125000, 8.000000, 8.000000, 8.125000, 7.875000, 8.000000, 7.875000, 8.250000, 7.875000, 8.000000, 8.000000, 8.125000, 8.000000, 8.125000, 8.125000, 8.000000, 7.625000, 7.875000, 8.000000, 7.875000, 8.000000, 8.000000, 7.875000, 8.000000, 8.125000, 7.875000, 7.750000, 7.625000, 2.250000, 7.500000, 7.375000, 7.500000, 7.375000, 7.375000, 7.500000, 7.500000, 7.500000, 7.375000, 7.625000, 7.500000, 7.625000, 7.500000, 7.500000, 7.375000, 7.500000, 7.750000, 7.375000, 7.500000, 7.500000, 7.625000, 7.500000, 7.500000, 7.750000, 7.625000, 7.500000, 7.500000, 7.625000, 7.625000, 7.375000, 7.375000, 7.500000, 7.500000, 7.500000, 7.500000, 7.500000, 7.500000, 7.375000, 7.375000, 7.375000, 7.500000, 7.375000, 7.375000, 7.500000, 7.750000, 7.625000, 7.500000, 7.500000, 7.375000, 7.375000, 7.500000, 7.250000, 7.250000, 7.375000, 7.250000, 7.250000, 7.125000, 7.125000, 7.250000, 7.500000, 7.250000, 7.500000, 7.125000, 8.875000, 8.625000, 8.375000, 8.500000, 8.125000, 8.250000, 8.250000, 8.125000, 8.375000, 8.250000, 8.125000, 8.125000, 8.250000, 8.125000, 8.000000, 7.750000, 7.750000, 7.875000, 7.750000, 7.750000, 7.875000, 8.000000, 7.875000, 7.625000, 8.000000, 7.875000, 7.750000, 7.750000, 7.750000, 7.625000, 7.750000, 7.750000, 7.875000, 7.625000, 7.750000, 7.875000, 8.000000, 7.625000, 7.625000, 7.750000, 7.750000, 7.750000, 7.625000, 7.500000, 7.625000, 7.500000, 7.500000, 7.500000, 7.375000, 7.500000, 7.500000, 7.500000, 7.625000, 7.500000, 7.500000, 7.375000, 7.625000, 7.500000, 7.375000, 7.375000, 7.500000, 7.125000, 7.250000, 7.250000, 8.875000, 8.750000, 8.625000, 8.250000, 8.500000, 8.375000, 8.250000, 8.375000, 8.250000, 8.125000, 8.375000, 8.375000, 8.125000, 8.125000, 8.250000, 8.125000, 8.125000, 8.000000, 8.000000, 8.125000, 8.125000, 8.000000, 8.000000, 8.125000, 7.875000, 8.125000, 8.000000, 8.000000, 8.000000, 7.750000, 7.875000, 7.875000, 7.750000, 7.625000, 7.625000, 7.750000, 7.625000, 7.750000, 7.625000, 7.750000, 7.750000, 7.375000, 7.500000, 7.500000, 7.750000, 7.500000, 7.375000, 7.250000, 7.375000, 7.500000, 7.500000, 7.375000, 7.375000, 7.500000, 7.375000, 7.375000, 7.500000, 7.625000, 7.500000, 7.500000, 7.500000, 7.375000, 7.375000, 7.250000, 9.250000, 9.125000, 9.000000, 9.000000, 9.000000, 9.000000, 8.750000, 8.875000, 8.875000, 9.000000, 9.000000, 8.875000, 8.875000, 9.000000, 8.750000, 8.875000, 8.750000, 8.875000, 8.875000, 9.000000, 9.000000, 9.000000, 8.750000, 8.750000, 8.625000, 8.875000, 8.875000, 8.625000, 8.625000, 8.750000, 8.625000, 8.500000, 8.875000, 8.500000, 8.750000, 8.625000, 8.750000, 8.625000, 8.500000, 8.875000, 8.750000, 8.875000, 8.750000, 8.625000, 8.625000, 8.625000, 8.750000, 8.750000, 8.625000, 8.750000, 8.750000, 9.000000, 8.750000, 8.875000, 8.875000, 8.625000, 9.000000, 8.875000, 8.875000, 8.750000, 9.000000, 8.750000, 8.750000, 8.750000, 8.750000, 8.500000, 8.375000, 8.250000, 8.250000, 8.250000, 8.250000, 8.125000, 8.250000, 8.125000, 8.375000, 8.250000, 8.125000, 8.125000, 8.250000, 8.125000, 8.125000, 8.125000, 7.875000, 8.000000, 8.125000, 7.875000, 8.000000, 8.000000, 7.750000, 8.000000, 24.750000, 8.125000, 8.000000, 8.125000, 8.125000, 8.125000, 3.250000, 8.250000, 3.250000, 8.000000, 15.125000, 8.000000, 7.875000, 8.000000, 7.875000, 7.875000, 7.750000, 7.875000, 7.625000, 7.875000, 7.875000, 7.875000, 8.000000, 7.875000, 8.000000, 7.750000, 7.875000, 8.000000, 7.625000, 7.500000, 7.750000, 7.625000, 7.750000, 7.625000, 7.750000, 8.000000, 9.000000, 9.375000};
  
  std::vector<double> values[4];
  values[0].assign(pedestals, pedestals+640);
  values[1].assign(sigmas, sigmas+640);
  values[2].assign(sigmaraws, sigmaraws+640);
  values[3].resize(640);

  std::vector<TH1F*> histos2;
  
  const char* names[4] = {"Pedestals", "Sigma", "RawSigma", "AverageSignal"};
  
  for (int ii=0; ii<4; ii++) {
    std::pair<TH1F*, TH1F*> coppia = VectorToHisto(values[ii], names[ii], "AMS-flight");
    TH1F* h = coppia.first;
    TH1F* h2 = coppia.second;
    if (ii!=3) {
      printf("%s %s %p %p\n", "AMS-flight", names[ii], h, h2);
    }
    histos.push_back(h);
    histos2.push_back(h2);
  }

  for (int ii=0; ii<3; ii++) {//not including the spread on the average signal
    histos.push_back(histos2.at(ii));
  }
  
  return;
}

std::pair<TH1F*, TH1F*> VectorToHisto(std::vector<double> values, const char* name, const char* filename, int nch, int nspread){

  TH1F* h = new TH1F(Form("%s_%s", name, filename), Form("%s_%s", name, filename), nch, 0, nch);
  TH1F* h2 = new TH1F(Form("%s_spread_%s", name, filename), Form("%s_spread_%s", name, filename), nspread*100, 0, nspread);

  TString name_ = name;
  TString filename_ = filename;
  
  auto nPoints = (int)(values.size());
  for(int i=0; i < nPoints; ++i) {
    /* if (name_ == "AverageSignal") { */
    /*   printf("%d) %f\n", i, values[i]); */
    /* } */
    h->Fill(i+0.5, values[i]);
    double val_for_comparison = values[i];
    if (filename_.Contains("LEF") || filename_.Contains("L0")) {
      if (i==0) printf("filename=%s so scaling values for comparison...\n", filename);
      val_for_comparison/=4.0;
    }
    h2->Fill(val_for_comparison);
  }

  std::pair<TH1F*, TH1F*> coppia = std::make_pair(h, h2);
			    
  return coppia;
}

void Summary(std::vector<TH1F*> histos, const char* filename, const char* nameout, int type) {
  
  int nva = 10;
  int nch_for_va = 64;
  double max_ped = 2000;
  double max_ave = 10.0;
  double max_sig = 10;
  double max_rsig = 10;

  if (type==1) {//AMS-L0
    nva = 16;
    nch_for_va = 64;
    max_ped = 4000;
    max_ave = 10.0;
    max_sig = 10;
    max_rsig = 10;
  }
  else {//type=0, AMS, FOOT/POX, ...
    nva=10;
    nch_for_va = 64;
    max_ped = 2000;
    max_ave = 10.0;
    max_sig = 10;
    max_rsig = 10;
  }

  int nch = nva*nch_for_va;
  
  static int count=-1;
  count++;

  TCanvas *c;

  if (((int)histos.size())!=0) {
    TLatex *comment=new TLatex(1-0.01,0.01,Form("%s",filename));
    comment->SetNDC();  
    comment->SetTextAngle(90);
    comment->SetTextAlign(11);
    comment->SetTextSize(0.025);

    c = new TCanvas(Form("c_%d", count), Form("%s_%d",filename,0),0*100,0*10,600,400);
    comment->Draw();
    c->Update();
    c->Divide(2,2);
    c->SetFillStyle(0);
    c->cd(1);
    gPad->SetFillStyle(0);
  
    c->cd(1);
    TH2F *fram=new TH2F(Form("fram_%d", count), Form("%s: pedestals",filename),nch,1,nch,max_ped,0,max_ped);
    fram->SetStats(0);
    fram->Draw();
    TH1F *isto = histos.at(0);
    //  printf("%p\n", isto);
    isto->Draw("samehist");
    gPad->Update();
    LinesVas(nva, ((int)(nch/nva)));
    gPad->Update();

    c->cd(2);
    gPad->SetFillStyle(0);
    TH2F *fram4=new TH2F(Form("fram4_%d", count), Form("%s: average signal",filename),nch,1,nch,max_ave,0,max_ave);
    fram4->SetStats(0);
    fram4->Draw();
    TH1F *isto4 = histos.at(3);
    /* isto4->Dump(); */
    /* for (int bb=0; bb<=isto4->GetNbinsX()+1; bb++){ */
    /*   printf("%d) %f\n", bb, isto4->GetBinContent(bb)); */
    /* } */
    isto4->Draw("samehist");
    gPad->Update();
    LinesVas(nva, ((int)(nch/nva)));
    gPad->Update();
    
    c->cd(3);
    gPad->SetFillStyle(0);
    TH2F *fram2=new TH2F(Form("fram2_%d", count), Form("%s: sigma",filename),nch,1,nch,max_sig,0,max_sig);
    fram2->SetStats(0);
    fram2->Draw();
    TH1F *isto2 = histos.at(1);
    isto2->Draw("samehist");
    gPad->Update();
    LinesVas(nva, ((int)(nch/nva)));
    gPad->Update();
  
    c->cd(4);
    gPad->SetFillStyle(0);
    TH2F *fram3=new TH2F(Form("fram3_%d", count), Form("%s: sigma raw",filename),nch,1,nch,max_rsig,0,max_rsig);
    fram3->SetStats(0);
    fram3->Draw();
    TH1F *isto3 = histos.at(2);
    isto3->Draw("samehist");
    gPad->Update();
    LinesVas(nva, ((int)(nch/nva)));
    gPad->Update();
  }
  else {
    c = new TCanvas("c", "",0*100,0*10,600,400);
  }
  
  gROOT->SetStyle("StyleWhite");
  c->Update();
  
  c->Print(nameout);

  if (((int)histos.size())==0) {
    /* if (comment) delete comment; */
    if (c) delete c;
    /* if (fram) delete fram; */
    /* if (fram2) delete fram2; */
    /* if (fram3) delete fram3; */
    /* if (isto) delete isto; */
    /* if (isto2) delete isto2; */
    /* if (isto3) delete isto3; */
  }

  return;
}

void LinesVas(int nva, int nch_for_va) {
  Double_t xmin,ymin,xmax,ymax;
  gPad->GetRangeAxis(xmin,ymin,xmax,ymax);
  TLine *line=new TLine();
  line->SetLineColor(kBlue);
  TText *text=new TText();
  text->SetTextAlign(21); // vert and hor. alignment: horiz centered, top
  text->SetTextColor(kBlue);
  line->SetLineStyle(3);
  for (Int_t va=1; va<=nva; va++) {// first and last separation lines are not drawn
    // line->SetLineStyle((va==10)?1:3);
    // if (va==11) {//this change the color from now on, since the `line` is always the same
    //   line->SetLineColor(kRed); text->SetTextColor(kRed);
    // }
    if (va*nch_for_va>xmin && va*nch_for_va<xmax) line->DrawLine(1+va*nch_for_va,ymin,1+va*nch_for_va,ymax);
    if (va*nch_for_va>xmin && va*nch_for_va<=xmax) text->DrawText((va-0.5)*nch_for_va,ymax+(ymax-ymin)*0.01,Form("%d",va));
  }

  if (line) delete line;
  if (text) delete text;
}

int InitStyle() {
  gROOT->SetStyle("Plain");

  TStyle *myStyle[2], *tempo;
  myStyle[0]=new TStyle("StyleWhite", "white");
  myStyle[1]=new TStyle("StyleBlack", "black");

  tempo=gStyle;
  Int_t linecol, bkgndcol, histcol;

  for(Int_t style=0; style<2; style++) {

    linecol=kWhite*style+kBlack*(1-style);
    bkgndcol=kBlack*style+kWhite*(1-style);
    histcol=kYellow*style+kBlack*(1-style); // was 95

    myStyle[style]->Copy(*tempo);

    myStyle[style]->SetCanvasBorderMode(0);
    myStyle[style]->SetCanvasBorderSize(1);
    myStyle[style]->SetFrameBorderSize(1);
    myStyle[style]->SetFrameBorderMode(0);
    myStyle[style]->SetPadBorderSize(1);
    myStyle[style]->SetStatBorderSize(1);
    myStyle[style]->SetTitleBorderSize(1);
    myStyle[style]->SetPadBorderMode(0);
    myStyle[style]->SetPalette(1,0);
    myStyle[style]->SetPaperSize(20,27);	
    myStyle[style]->SetFuncColor(kRed);
    myStyle[style]->SetFuncWidth(1);
    myStyle[style]->SetLineScalePS(1); 
    myStyle[style]->SetCanvasColor(bkgndcol);
    myStyle[style]->SetAxisColor(linecol,"XYZ");
    myStyle[style]->SetFrameFillColor(bkgndcol);
    myStyle[style]->SetFrameLineColor(linecol);
    myStyle[style]->SetLabelColor(linecol,"XYZ");
    myStyle[style]->SetPadColor(bkgndcol);
    myStyle[style]->SetStatColor(bkgndcol);
    myStyle[style]->SetStatTextColor(linecol);
    myStyle[style]->SetTitleColor(linecol,"XYZ");
    myStyle[style]->SetTitleFillColor(bkgndcol);
    myStyle[style]->SetTitleTextColor(linecol);
    myStyle[style]->SetLineColor(linecol);
    myStyle[style]->SetMarkerColor(histcol);
    myStyle[style]->SetTextColor(linecol);

    myStyle[style]->SetGridColor((style)?13:kBlack);
    myStyle[style]->SetHistFillStyle(1001*(1-style));
    myStyle[style]->SetHistLineColor(histcol);
    myStyle[style]->SetHistFillColor((style)?bkgndcol:kYellow);
    myStyle[style]->SetOptDate(22);
    myStyle[style]->GetAttDate()->SetTextColor(linecol);
    myStyle[style]->GetAttDate()->SetTextAngle(90);
    myStyle[style]->GetAttDate()->SetTextSize(0.01);
    myStyle[style]->SetDateY(1);
    myStyle[style]->SetDateX(1);
  }

  myStyle[1]->cd();

  gROOT->ForceStyle();

  //if (myStyle[0]) delete myStyle[0];
  //if (myStyle[1]) delete myStyle[1];

  return 0;
}

void Comparison(std::vector<TH1F*> histos[3], const char* nameout) {

  static int count=-1;
  count++;

  Color_t colors[11] = {kRed+2, kGreen+2, kBlue+2, kCyan, kBlack, kPink, kRed, kGreen, kBlue, kOrange, kYellow};
  if ((int)(histos[0].size())>11) {
    printf("I'll crash...\n");
  }
  
  TCanvas *c;
    
  c = new TCanvas(Form("comp_c_%d", count), Form("%d",0),0*100,0*10,600,400);
  c->Update();
  c->Divide(2,2);
  c->SetFillStyle(0);
  c->cd(1);
  gPad->SetFillStyle(0);
  
  c->cd(1);
  TH2F *fram=new TH2F(Form("comp_fram_%d", count), Form("pedestals"), 200, 0, 2000, 100, 0, 100);
  fram->GetXaxis()->SetTitle("ADC");
  fram->GetYaxis()->SetTitle("Channels");
  fram->SetStats(0);
  fram->Draw();
  for (int ii=0; ii<(int)(histos[0].size()); ii++) {
    histos[0].at(ii)->SetFillStyle(0);
    histos[0].at(ii)->SetLineColor(colors[ii]);
    histos[0].at(ii)->SetLineWidth(3);
    histos[0].at(ii)->Rebin(pow(2, 10));
    histos[0].at(ii)->Draw("samehist");
  }
  gPad->Update();
  
  c->cd(3);
  gPad->SetFillStyle(0);
  TH2F *fram2=new TH2F(Form("comp_fram2_%d", count), Form("sigma"), 1000, 0, 10, 450, 0, 450);
  fram2->GetXaxis()->SetTitle("ADC");
  fram2->GetYaxis()->SetTitle("Channels");
  fram2->SetStats(0);
  fram2->Draw();
  for (int ii=0; ii<(int)(histos[1].size()); ii++) {
    histos[1].at(ii)->SetFillStyle(0);
    histos[1].at(ii)->SetLineColor(colors[ii]);
    histos[1].at(ii)->SetLineWidth(3);
    histos[1].at(ii)->Rebin(pow(2, 2));
    histos[1].at(ii)->Draw("samehist");
  }
  gPad->Update();
  
  c->cd(4);
  gPad->SetFillStyle(0);
  TH2F *fram3=new TH2F(Form("comp_fram3_%d", count), Form("sigma raw"), 1500, 0, 15, 200, 0, 200);
  fram3->GetXaxis()->SetTitle("ADC");
  fram3->GetYaxis()->SetTitle("Channels");
  fram3->SetStats(0);
  fram3->Draw();
  for (int ii=0; ii<(int)(histos[2].size()); ii++) {
    histos[2].at(ii)->SetFillStyle(0);
    histos[2].at(ii)->SetLineColor(colors[ii]);
    histos[2].at(ii)->SetLineWidth(3);
    histos[2].at(ii)->Rebin(pow(2, 2));
    histos[2].at(ii)->Draw("samehist");
  }
  gPad->Update();

  gROOT->SetStyle("StyleWhite");
  c->Update();

  c->Print(nameout);

  return;
}

//-------------------------------------------------------------------------------------------------------------------------

//----------------------------------------------------------
//  LV3 Huffman decoding
//----------------------------------------------------------

int16 calculate_CRC16(int16 *dat, int16 len) {

  int i;
  int16 CRC = 0xFFFF;
  int16 g = 0x1021;

  for (i=0; i<len; i++) {
    int j;
    int16 d = dat[i];
    for (j=0; j<16; j++) {
      if ((CRC ^ d) & 0x8000) CRC = (CRC << 1) ^ g;
      else                    CRC = (CRC << 1);
      d <<= 1;
    }
  }
  return CRC;
}

//----------------------------------------------------------
//  Function: __huffman_bit
//      process data bit in huffman method
//
//  Inputs:
//      data ---- Data word
//      bit ----- bit
//
//  Return:
//      Output word
//----------------------------------------------------------
static uint16_t __huffman_bit(uint16_t *data, int bit) {
  return (data[bit >> 4] & (1 << (15 - (bit & 15)))) >> (15 - (bit & 15));
}

//----------------------------------------------------------
//  Function: __huffman_decode_init
//      Initialize huffman decode table
//
//  Inputs:
//      None
//
//  Return:
//      None
//----------------------------------------------------------
static void __huffman_decode_init(void) {

  uint8_t  *ptr;
  uint     v;
  uint16_t i, n, m, pos, vcode, lcode;

  for(i=0; i<16; i++){                                        // code length
    for (n=0;  n<256; n++) {                                // position for a given length
      huffman_decode_table[i][n] = 0xFFFF;                // undefine code
    }
  }

  m = 0;
  for(i=0; i<16; i++){
    if ( i < 9 ) huffman_first_code[i] = 1<<(i+1);
    else         huffman_first_code[i] = 0;

    pos = 0;
    for (n=0;  n<256; n++) {
      v = huffman_codes[n];                               // Huffman codes
      vcode = v >> 16;                                    // encoded bit pattern
      lcode = v & 0xFFFF;                                 // number of bits in the pattern
      if ( m < lcode ) m = lcode;

      if( lcode == (i+1) ) {
	if ( pos == 0 ) huffman_first_code[i] = vcode;
	huffman_decode_table[i][pos++] = n;             // fill decoding table
      }
    }
  }

  // insert end symbol
  huffman_decode_table[m][0] = 256;                     // end symbol code

  // define swap rules
  ptr = (uint8_t *) &vcode;
  vcode = 0x55AA;
  lcode = (ptr[1] << 8) | ptr[0];
  if ( lcode == vcode ) huffman_swap = 1;

  // Initialized
  huffman_inited = 1;
}

//----------------------------------------------------------
//  Function: __huffman_decode
//      Decode function for huffman encoded data
//
//  Inputs:
//      istream ---- Input stream
//      length ----- Number of bytes in input stream
//      dest ------- Destination for output data
//
//  Return:
//      Number of bytes in output stream
//----------------------------------------------------------
static uint16_t __huffman_decode(uint16_t *istream, uint16_t length, uint16_t **dest ) {
  uint8_t  *ostream;
  uint16_t l, v, dv, nb = 0;
  uint     base = 0;

  // Check for initialize
  if ( huffman_inited == 0 ) __huffman_decode_init();

  //
  // decoding ends when all input words are processed
  // or END symbol - (12 zero bits) is identified
  //
  ostream = (uint8_t *) &huffman_restore[1];

  while( 1 ) {
    // get encoded value
    for(l = 0, v = __huffman_bit(istream, (l + base)); v < huffman_first_code[l]; v = (v << 1) | __huffman_bit(istream, (++l + base)));

    base += (l + 1);

    // BASE exceeding length means remaing bits are all ZEROes
    if ( base > (length << 4) ) break;                    // total decoded length <= input length

    dv = huffman_decode_table[l][v - huffman_first_code[l]];
    if      ( dv <  256 ) ostream[nb++] = dv;   // valid byte - continue decoding
    else if ( dv == 256 );                      // end symbol - normal termination
    else                  nb = 0;               // invalid byte - abnormal termination
  }

  // if the encoded stream is only partially processed
  if ( base < ((length<<4)-12) ) nb = 0;

  // swap bytes if needed
  if ( huffman_swap == 1 ) {
    ostream = (unsigned char *) &huffman_restore[0];
    for (l=0; l<nb; l+=2) {
      ostream[l  ] = ostream[l+3];
      ostream[l+1] = ostream[l+2];
    }
    *dest = (uint16_t *) &huffman_restore[0];
  }
  else {
    *dest = (uint16_t *) &huffman_restore[1];
  }

  return nb;
}

//----------------------------------------------------------
//  Function: __decode_daq_data (calibration, config and HK)
//      Decode function for calibration data
//
//  Inputs:
//      block ---- AMS block
//      utime ---- Time in microseconds
//
//  Return:
//      None
//----------------------------------------------------------
static void __decode_daq_data(AMSBlock *block, uint64_t utime)
{
  int s;
  uint8_t *p;
  uint64_t mtime = utime;    // Keep block second time
  int i = 0, j;
  int n, m;
  uint32_t id;
  uint16_t tag;
  char fields[100];

  // Get block data
  s = block->DataCount;
  p = abiDataPtr8(block);

  // jmdc sub-block size
  n  = (p[i++] << 8);
  n |= p[i++];
  if ( (n & 0x8000) != 0 ) {
    n  = (n & 0x7FFF) << 16;
    n |= (p[i++] << 8);
    n |= p[i++];
  }
  if ( n > s ) {
    //        LogWarning("[%07d] %s> Wrong JMDC sub-block size: size=%d, jmdc=%d\n", file_no, block_time, s, n);
    printf("%s> Wrong JMDC sub-block size: size=%d, jmdc=%d\n", block_time, s, n);
    return;
  }

  // sub-detector block
  j  = i + n;
  m  = (p[j++] << 8);
  m |= p[j++];
  if ( (m & 0x8000) != 0 ) {
    m  = (n & 0x7FFF) << 16;
    m |= (p[j++] << 8);
    m |= p[j++];
  }

  // Check size
  if ( (j + m) != s ) {
    //        LogWarning("[%07d] %s> Wrong DAQ data size: size=%d, jmdc=%d, subd=%d\n", file_no, block_time, s, n, m);
    printf("[%07d] %s> Wrong DAQ data size: size=%d, jmdc=%d, subd=%d\n", block_time, s, n, m);
    return;
  }

  // Get msec time
  mtime += ((p[i+2] << 8) | p[i+3]) * 1000;

  // Calibration runID
  tag = (p[i+4] << 8) | p[i+5];
  id  = (p[i+6] << 24) | (p[i+7] << 16) | (p[i+8] << 8) | p[i+9];
  i   = (block->DataType - 6);
  if ( i <= 2 && id != old_daq_id ) {
    char *snames[3] = {
      "DAQ_Info,type=Cal",
      "DAQ_Info,type=Cfg",
      "DAQ_Info,type=Hkd",
    };

    snprintf(fields, sizeof(fields), "RunTag=%di,RunID=%ldi", tag, id);
    //        db_client_data(client, snames[i], fields, utime, NO_DEBUG);

    old_daq_id = id;
  }

  // Find sub-detector node address
  n = ((p[j] & 0x3F) << 3) | (p[j+1] >> 5);

  //  Removed on Dec.5, 2019.
  //  j and m point to the header word and should not changed for long datatype
  //     // Get sub-detector data type, data size and pointer
  //     if ( (p[j+1] & 0x1F) == 0x1F ) {
  //         j += 6;
  //         m -= 6;
  //     }

  // Call decode functions for each sub-detector
  switch (n) {
    // JLV1
  case JLV1_A ... JLV1_S:
    jlv1_funcs[i](block, mtime, id, n, m, &p[j]);
    break;

    // ECAL
  case JINF_E_0_A ... JINF_E_1_S:
  case ETRG_0_A ... ETRG_1_S:
  case EDR_0_0_A ... EDR_1_5_P:
    ecal_funcs[i](block, mtime, id, n, m, &p[j]);
    break;

    // RICH
  case JINF_R_0_A ... JINF_R_1_S:
  case RDR_0_00 ... RDR_1_11:
    rich_funcs[i](block, mtime, id, n, m, &p[j]);
    break;

    // TOF
  case SDR_0_A ... SDR_3_S:
    tof_funcs[i](block, mtime, id, n, m, &p[j]);
    break;

    // Tracker
  case JINF_T_0_A ... JINF_T_7_S:
  case TDR_0_00_A ... TDR_7_11_B:
    tracker_funcs[i](block, mtime, id, n, m, &p[j]);
    break;

    // TRD
  case JINF_U_0_A ... JINF_U_1_S:
  case UDR_0_0_A ... UDR_1_5_P:
    trd_funcs[i](block, mtime, id, n, m, &p[j]);
    break;
  }
}

//----------------------------------------------------------
//  Function: __decode_gtsn_table
//      Decode function for JMDC GTSN table
//
//  Inputs:
//      block ---- AMS block
//      utime ---- Time in microseconds
//
//  Return:
//      None
//----------------------------------------------------------
static void __decode_gtsn_table(AMSBlock *block, uint64_t utime)
{
  uint8_t *p;
  int i;
  int l;
  int n;
  int b;
  int s;
  int w;
  int8_t ah, al,wh, wl;
  char fields[200];

  // Get block info
  l = block->DataCount;
  p = abiDataPtr8(block);

  // Loop on table
  for ( i = 0; i < l; ) {
    // get node ID
    n = p[i] >> 4;
    if ( n > 11 ) {
      //	  LogError("[%07d] %s> Wrong GTSN table, p[0]=%02X, p[1]=%02X, size=%d, i=%d\n",
      //         file_no, block_time, p[i], p[i+1], l, i);
      printf("%s> Wrong GTSN table, p[0]=%02X, p[1]=%02X, size=%d, i=%d\n",
	     block_time, p[i], p[i+1], l, i);
      return;
    }

    // Get bus number
    b = (p[i++] & 0xF);
    if ( n < 10 ) b--;      // TTCS bus number starting from 0, others from 1
    if ( i >= l ) goto __gtsn_table_err;

    // Get sensor number
    s = (p[i++] & 0x3F);
    if ( i >= l ) goto __gtsn_table_err;

    // Check sensor number
    if ( s >= ds_nsens[n][b] ) {
      //	  LogError("[%07d] %s> Wrong sensor number, node=%s (%s), bus=%d, sensor=%d (%d)\n", file_no, block_time,
      //                      node_names[gtsn_nodes[n].node], node_names[ds->node], b, s, ds->nsens);
      //	  printf("%s> Wrong sensor number, node=%s (%s), bus=%d, sensor=%d (%d)\n", block_time,
      //		 node_names[gtsn_nodes[n].node], node_names[ds->node], b, s, ds->nsens);
      return;
    }

    // Get limits
    al = (int8_t) p[i++];
    if ( i >= l ) goto __gtsn_table_err;
    wl = (int8_t) p[i++];
    if ( i >= l ) goto __gtsn_table_err;
    wh = (int8_t) p[i++];
    if ( i >= l ) goto __gtsn_table_err;
    ah = (int8_t) p[i++];
    if ( i >= l ) goto __gtsn_table_err;

    // Save limit for check
    ds_limits[n][b][s][LTA] = (float) al;
    ds_limits[n][b][s][LTW] = (float) wl;
    ds_limits[n][b][s][HTW] = (float) wh;
    ds_limits[n][b][s][HTA] = (float) ah;

    // Write to DB
    snprintf(fields, sizeof(fields), "low_alarm=%d,low_warning=%d,high_warning=%d,high_alarm=%d",
	     al, wl, wh, ah);
    //        db_client_data(client, ds_tags[n][b][s], fields, utime, NO_DEBUG);

    // Get size of block
    w = (p[i] << 8) | p[i+1];
    i += (w + 2);
    if ( i >= l ) goto __gtsn_table_err;
    w = (p[i] << 8) | p[i+1];
    i += (w + 2);
    if ( i > l ) goto __gtsn_table_err;
  }

  // End
  return;

 __gtsn_table_err:
  //    LogError("[%07d] %s> In completed GTSN table, size=%d, i=%d\n", file_no, block_time, l, i);
  printf("%s> In completed GTSN table, size=%d, i=%d\n", block_time, l, i);
}

//----------------------------------------------------------
//  Function: __decode_jbux_pointers
//      Decode function for JMDC JBUX pointers
//
//  Inputs:
//      block ---- AMS block
//      utime ---- Time in microseconds
//
//  Return:
//      None
//----------------------------------------------------------
static void __decode_jbux_pointers(AMSBlock *block, uint64_t utime)
{
  int i;
  uint32_t *p = abiDataPtr32(block);
  int jmdc = (block->NodeAddress & 3);
  char name[100], field[100];

  // Wrong size
  if ( block->DataCount != 16 ) return;

  // Swap
  for ( i = 0; i < 4; i++ ) p[i] = bswap_32(p[i]);

  // Pointers
  sprintf(name, "JMDC,name=JBUX_pointers,ref=JMDC_%d", jmdc);
  sprintf(field, "write=%d,read=%d,erase=%d,free=%d", p[0], p[1], p[2], p[3]);
  //    db_client_data(client, name, field, utime, NO_DEBUG);

  // Files
  uint32_t remain = (p[0] < p[1] ? (p[0] + 0x6FFF0 - p[1]) : (p[0] - p[1]));
  uint32_t total  = (p[1] < p[2] ? (p[1] + 0x6FFF0 - p[2]) : (p[1] - p[2]));
  uint32_t free   = (0x6FFF0 - remain);
  sprintf(name, "JMDC,name=JBUX_files,ref=JMDC_%d", jmdc);
  sprintf(field, "remain=%d,total=%d,free=%d", remain, total, free);
  //    db_client_data(client, name, field, utime, NO_DEBUG);
}

//----------------------------------------------------------
//  Function: __decode_utt_sts
//      Decode function for JMDC UTTPS protection and recovery status
//
//  Inputs:
//      block ---- AMS block
//      utime ---- Time in microseconds
//
//  Return:
//      None
//----------------------------------------------------------
static void __decode_utt_sts(AMSBlock *block, uint64_t utime)
{
  uint8_t *p = abiDataPtr8(block);
  int jmdc = (block->NodeAddress & 3);
  char name[100], field[100];
  uint16_t w;

  // Wrong size
  if ( block->DataCount != 32 ) return;

  // Reference result (sent from calculation directly)
  if ( block->CommandTag == 0x0E10 ) {
    // Reference current
    w = ((p[14] << 8) | p[15]);
    sprintf(name, "TTCS_UTTPS,name=reference,group=trent_result", jmdc);
    sprintf(field, "value=%.6f,stage=%d,status=%d", (w / 64.0), p[20], p[21]);
    //        db_client_data(client, name, field, utime, NO_DEBUG);

    // Orbit Average
    w = ((p[16] << 8) | p[17]);
    sprintf(name, "TTCS_UTTPS,name=Orbit_Average,group=trent_result", jmdc);
    sprintf(field, "value=%.6f,stage=%d,status=%d", (w / 64.0), p[20], p[21]);
    //        db_client_data(client, name, field, utime, NO_DEBUG);
  }

  // Status by polling and result
  // Reference current
  w = ((p[14] << 8) | p[15]);
  sprintf(name, "TTCS_UTTPS,name=reference,group=trent_status", jmdc);
  sprintf(field, "value=%.6f,stage=%d,status=%d", (w / 64.0), p[20], p[21]);
  //    db_client_data(client, name, field, utime, NO_DEBUG);

  // Orbit Average
  w = ((p[16] << 8) | p[17]);
  sprintf(name, "TTCS_UTTPS,name=Orbit_Average,group=trent_status", jmdc);
  sprintf(field, "value=%.6f,stage=%d,status=%d", (w / 64.0), p[20], p[21]);
  //    db_client_data(client, name, field, utime, NO_DEBUG);
}

//----------------------------------------------------------
//  Function: __decode_event
//      Decode function for event data
//
//  Inputs:
//      block ---- AMS block
//      utime ---- Time in microseconds
//
//  Return:
//      None
//----------------------------------------------------------
static void __decode_event(AMSBlock *block, uint64_t utime)
{
  uint64_t jmdc_time = utime;    // Keep block second time
  uint64_t gps_time = 0;
  int i = 0, j;
  int m, n, l, idx;
  char fields[200], name[100];
  int gps_fmt = 0, gps_val = 0;
  int jinj_size, trk_size;

  // Write summary data every seconds when block time changed
  char *sd_names[NSD] = {"JINJ_size", "JLV1_size", "TRD_size", "Tracker_size", "RICH_size", "ECAL_size", "TOF_size"};
  if ( old_time == 0 ) old_time = block->DataTime;
  if ( block->DataTime != old_time ) {
    // Time difference
    if ( ntime_data != 0 ) {
      snprintf(fields, sizeof(fields), "time_diff=%.3f", ((double) time_diff / 1000000.0 / ntime_data));
      //            db_client_data(client, "DAQ_Info,type=Evt", fields, old_evt_time, NO_DEBUG);
    }

    if ( evt_cnt != 0 ) {
      // Total size and compressed rate
      j  = snprintf(fields, sizeof(fields), "Block_size=%.2f", ((double) evt_block_size / evt_cnt));
      j += snprintf(&fields[j], (sizeof(fields) - j), ",Compressed_size=%.2f", ((double) evt_comp_size / evt_cnt));
      j += snprintf(&fields[j], (sizeof(fields) - j), ",Compress_factor=%.2f", ((double) evt_comp_size / evt_sizes[JINJ]));
      j += snprintf(&fields[j], (sizeof(fields) - j), ",Event_rate=%di", evt_cnt);
      j += snprintf(&fields[j], (sizeof(fields) - j), ",Data_rate(compressed)=%.2f", ((double) evt_block_size / 1048576.0));
      j += snprintf(&fields[j], (sizeof(fields) - j), ",Data_rate=%.2f", ((double) evt_sizes[JINJ] / 1048576.0));
      //            db_client_data(client, "DAQ_Info,type=Evt", fields, old_evt_time, NO_DEBUG);

      // Event sizes
      for ( j = 0, m = 0; j < NSD; j++ ) {
	m += snprintf(&fields[m], (sizeof(fields) - m), "%s%s=%.2f",
		      (m == 0 ? "" : ","), sd_names[j], ((double) evt_sizes[j] / evt_cnt));
      }
      //            db_client_data(client, "DAQ_Info,type=Evt", fields, old_evt_time, NO_DEBUG);

      // Event number and error count
      snprintf(fields, sizeof(fields), "EventNb=%di,EventErrs=%di", old_evt_no, nerrs);
      //            db_client_data(client, "DAQ_Info,type=Evt", fields, old_evt_time, NO_DEBUG);

      // Trigger time
      sprintf(fields, "trig_time_max=%.2f,trig_time_min=%.2f,trig_time_avg=%.2f",
	      (trig_time_max * 0.64), (trig_time_min * 0.64), ((trig_time_sum / evt_cnt) * 0.64));
      //            db_client_data(client, "DAQ_Info,type=Trigger", fields, old_evt_time, NO_DEBUG);

      // Event sizes (new create data set, 2020.03.22)
      sprintf(name, "DAQ_Info,type=Evt,group=Event_Size,name=Block_size");
      sprintf(fields, "size=%.2f", ((double) evt_block_size / evt_cnt));
      //            db_client_data(client, name, fields, old_evt_time, NO_DEBUG);
      sprintf(name, "DAQ_Info,type=Evt,group=Event_Size,name=Compressed_size");
      sprintf(fields, "size=%.2f", ((double) evt_comp_size / evt_cnt));
      //            db_client_data(client, name, fields, old_evt_time, NO_DEBUG);
      for ( j = 0; j < NSD; j++ ) {
	sprintf(name, "DAQ_Info,type=Evt,group=Event_Size,name=%s", sd_names[j]);
	sprintf(fields, "size=%.2f", ((double) evt_sizes[j] / evt_cnt));
	//                db_client_data(client, name, fields, old_evt_time, NO_DEBUG);
      }

      // ... Will add later for all event errors
      // // JINJ Errors
      // for ( j = 0; j < 4; j++ ) {
      // if ( (jinj_errs[j][0] + jinj_errs[j][1] + jinj_errs[j][2]) == 0 ) continue;
      // sprintf(name, "DAQ_Info,type=Evt,group=Event_Errors,node=JINJ-%d", j);
      // sprintf(fields, "crc_errs=%di,build_errs=%di,link_errs=%di",
      // jinj_errs[j][0], jinj_errs[j][1], jinj_errs[j][2]);
      // db_client_data(client, name, fields, old_evt_time, NO_DEBUG);
      // }

      /*
      // SubD data results
      sum_jinfu_event(evt_cnt, old_evt_time);
      sum_jinft_event(evt_cnt, old_evt_time);
      sum_jinfr_event(evt_cnt, old_evt_time);
      sum_jinfe_event(evt_cnt, old_evt_time);
      sum_sdr_event(evt_cnt, old_evt_time);
      */
    }

    // End
    memset(evt_sizes, 0, sizeof(evt_sizes));
    old_time = block->DataTime;
    evt_cnt = 0;
    nerrs   = 0;
    memset(jinj_errs, 0, sizeof(jinj_errs));
    trig_time_max  = 0;
    trig_time_min  = 0x10000;
    trig_time_sum  = 0;
    evt_block_size = 0;
    evt_comp_size  = 0;
    time_diff  = 0;
    ntime_data = 0;
  }

  /*
  // Write size distributions
  if ( old_fileno < 0 ) old_fileno = file_no;
  if ( old_fileno != file_no ) {
  // JINJ size distribution
  for ( j = 0; j < JINJ_BINS; j++ ) {
  sprintf(name, "DAQ_Info,type=Evt,group=EventSize,name=JINJ_size,bin=%04d",
  (jinj_bin_min + (j * jinj_bin_step)));
  sprintf(fields, "bin_value=%di", jinj_bins[j]);
  //            db_client_data(client, name, fields, utime, NO_DEBUG);
  }
  memset(jinj_bins, 0, sizeof(jinj_bins));

  // Tracker size distribution
  for ( j = 0; j < TRK_BINS; j++ ) {
  sprintf(name, "DAQ_Info,type=Evt,group=EventSize,name=Tracker_size,bin=%04d",
  (trk_bin_min + (j * trk_bin_step)));
  sprintf(fields, "bin_value=%di", trk_bins[j]);
  //            db_client_data(client, name, fields, utime, NO_DEBUG);
  }

  // Clear
  memset(jinj_bins, 0, sizeof(jinj_bins));
  memset(trk_bins, 0, sizeof(trk_bins));
  old_fileno = file_no;

  // Call Tracker size distribution summary
  sum_jinft_distr(utime);
  }
  */

  // Get current block data
  int s = block->DataCount;
  uint8_t *p = abiDataPtr8(block);
  evt_block_size += s;

  // jmdc sub-block size
  n  = (p[i++] << 8);
  n |= p[i++];
  if ( (n & 0x8000) != 0 ) {
    n  = (n & 0x7FFF) << 16;
    n |= (p[i++] << 8);
    n |= p[i++];
  }

  // Datatype of the JMDC sub-block
  int type = (p[i+1] & 0x1F);

  // Get msec time
  jmdc_time += ((p[i+2] << 8) | p[i+3]) * 1000;
  old_evt_time = jmdc_time;       // Remember the last event time used as time to write into DB

  // runID
  uint16_t tag = (p[i+4] << 8) | p[i+5];
  uint32_t id  = (p[i+6] << 24) | (p[i+7] << 16) | (p[i+8] << 8) | p[i+9];
  if ( id != old_evt_id ) {
    snprintf(fields, sizeof(fields), "RunTag=%di,RunID=%ldi", tag, id);
    //        db_client_data(client, "DAQ_Info,type=Evt", fields, jmdc_time, NO_DEBUG);

    old_evt_id = id;
  }

  // Event number
  uint32_t evt = (p[i+10] << 24) | (p[i+11] << 16) | (p[i+12] << 8) | p[i+13];
  m = (evt - old_evt_no);
  if ( old_evt_no != 0xFFFFFFFF && (m > 1 || (m < 0 && evt > 1)) ) {
    if ( m < 0 ) m = evt;
    snprintf(fields, sizeof(fields), "EvtLost=%di", m);
    //        db_client_data(client, "DAQ_Info,type=Evt", fields, jmdc_time, NO_DEBUG);
  }
  old_evt_no = evt;
  evt_cnt++;

  // LV3 data in JMDC sub-block
  j = i + 14;
  if ( type == 0x11 || type == 0x13 ) {
    // Number of bytes followed
    m = (p[j++] & 0x1F);

    // Check size
    if ( (j + m) <= (i + n) ) {
      // Time from previous trigger
      l = ((p[j+7] << 8) | p[j+8]);
      if ( l < trig_time_min ) trig_time_min = l;
      if ( l > trig_time_max ) trig_time_max = l;
      trig_time_sum += l;

      // End of LV3 data
      j += m;
    }
    else j = (i + n);       // Skip all data
  }

  // GPS data in JMDC sub-block
  if ( type == 0x12 || type == 0x13 ) {
    // Number of bytes followed
    m = (p[j] & 0x1F);

    // Check size
    if ( (j + m) <= (i + n) ) {
      // Format
      type = (p[j+1] & 7);
      gps_fmt = ((type + 1) % 3);
      //             utc_fmt = ((type + 1) / 3);

      // Valid
      gps_val = ((p[j+1] >> 5) & 1);

      // GPS time
      gps_time = 0LL;
      if ( gps_fmt > 1 ) {
	gps_time  = ((p[j+4] << 24) | (p[j+5] << 16) | (p[j+6] << 8) | p[j+7]);
	gps_time *= 1000000LL;
      }
    }
  }

  // Loop on JINJ sub-blocks
  for ( jinj_size = 0, trk_size = 0, i += n; i < s; i += n ) {
    // size (short format only)
    n  = (p[i] << 8) | p[i+1];
    evt_comp_size += n;             // Compressed event size
    i += 2;

    // Node address
    uint16_t node = ((p[i] << 8) | p[i+1]);
    int type = (node & 0x1F);       // Short format only
    node = ((node >> 5) & 0x1FF);

    // JINJ data pointer
    uint16_t *data = (uint16_t *) &p[i+2];
    m = (n - 1) >> 1;               // Exclude header

    // Swap data
    for ( j = 0; j < m; j++ ) data[j] = bswap_16(data[j]);

    // Compressed data
    if ( type == 4 ) {
      // huffman decode (returned is the pointer to huffman_restore area)
      m = (__huffman_decode((uint16_t *) &p[i+2], m, &data) + 1) / 2;
    }

    // Check CRC
    if ( calculate_CRC16(data, m) != 0 ) {
      nerrs++;
      jinj_errs[node & 3][0] += 1;        // CRC error
      continue;
    }

    // JINJ Error status
    if ( (data[m-2] & 0x0600) != 0 ) {
      nerrs++;
      jinj_errs[node & 3][1] += 1;        // Building error
    }

    // Set JINJ size
    jinj_size += (m << 1);                  // in bytes
    evt_sizes[JINJ] += (m << 1);

    // Loop on slaves
    for ( j = 1, l = 0; j < (m - 2); j += (l + 1) ) {
      // Get size of sub-block
      l = data[j];
      if ( (l + j) >= (m - 2) ) break;

      // Wrong data
      idx = data[j+l] & 0x1F;
      if ( idx > 23 ) {
	jinj_errs[node & 3][2] += 1;    // Link error
	break;
      }

      // Add size
      evt_sizes[sl_idx[idx]] += (l << 1);

      // Tracker size
      if ( sl_idx[idx] == TRK ) trk_size += (l << 1);

      // LV1 data
      if ( (idx == 14 || idx == 15) && gps_val != 0 && gps_time != 0 ) {
	ntime_data++;
	uint64_t lv1_time = ((((data[j+7] >> 8) | (data[j+8] << 8)) - 1) * 1000000);
	lv1_time += ((((uint64_t) data[j+7] & 0x00FF) << 32) | (data[j+6] << 16) | data[j+5]) * 64 / 100;
	lv1_time += gps_time;
	time_diff += (jmdc_time - lv1_time);
      }

      // SubD event data
      if ( funcs[idx] != NULL ) {
	funcs[idx](subd_index[idx], &data[j+1], l);
      }
    }
  }

  // JINJ size distribution
  for ( i = 0; i < JINJ_BINS; i++ ) {
    // Last bin
    if ( i == (JINJ_BINS - 1) ) {
      jinj_bins[JINJ_BINS - 1] += 1;
      break;
    }

    // Check bin
    if ( jinj_size <= (jinj_bin_min + (i * jinj_bin_step)) ) {
      jinj_bins[i] += 1;
      break;
    }
  }

  // Tracker size distribution
  for ( i = 0; i < TRK_BINS; i++ ) {
    // Last bin
    if ( i == (TRK_BINS - 1) ) {
      trk_bins[TRK_BINS - 1] += 1;
      break;
    }

    // Check bin
    if ( trk_size <= (trk_bin_min + (i * trk_bin_step)) ) {
      trk_bins[i] += 1;
      break;
    }
  }
}

//----------------------------------------------------------
//  Function: decode_jmdc
//      Decode function for jmdc
//
//  Inputs:
//      block ---- AMS block
//      utime ---- Time in microseconds
//
//  Return:
//      None
//----------------------------------------------------------
void decode_jmdc(AMSBlock *block, uint64_t utime)
{
  // Datatypes
  switch ( block->DataType ) {
    // Science data
  case 0x05:
    __decode_event(block, utime);
    break;

    // DAQ data (calibration, configuration and HK)
  case 0x06 ... 0x08:
    __decode_daq_data(block, utime);
    break;

    // JBUX pointers
  case 0x1F05A4:
    __decode_jbux_pointers(block, utime);
    break;

    // GTSN table read
  case 0x1F0721:
    __decode_gtsn_table(block, utime);
    break;

    // UG MFDP data
  case 0x1F0712:
    decode_ug_mfdp(block, utime);
    break;

    // UG HK data
  case 0x1F0713:
    decode_ug_hkdata(block, utime);
    break;

    // UG CHD params
  case 0x1F0715:
    decode_ug_chdparam(block, utime);
    break;

    // UG Protection & Recovery parameters
  case 0x1F075D:
    __decode_utt_sts(block, utime);
    break;

    // CHD data
  case 0x1FEED3:
  case 0x1FEEDA:
    decode_jmdc_chd(block, utime);
    break;
  }
}

//----------------------------------------------------------
//  Function: decode_fep
//      Decode function for FEP servers
//
//  Inputs:
//      block ---- AMS block
//      utime ---- Time in microseconds
//
//  Return:
//      None
//----------------------------------------------------------
void decode_fep(AMSBlock *block, uint64_t utime)
{
  uint8_t *p = abiDataPtr8(block);

  //    printf("DT: 0x%x\n", block->DataType);
    
  // Datatypes
  switch (block->DataType) {
    // CHD
  case 0x1FEED3:
  case 0x1FEEDA:
    // Change node address to JMDC ID inside CHD
    block->NodeAddress = 0xC + (p[2] >> 6);
    decode_jmdc_chd(block, utime);
    break;
  }
}

//----------------------------------------------------------
//  Function:
//      Function to compare two data for sort
//
//  Inputs:
//      a ---- first data
//      b ---- second data
//
//  Return:
//      -1 : a less than b
//       0 : a equal to b
//       1 : a greater than b
//----------------------------------------------------------
static int __compare(const void *a, const void *b)
{
  if      ( *((double *) a) < *((double *) b) ) return -1;
  else if ( *((double *) a) > *((double *) b) ) return 1;
  return 0;
}

//----------------------------------------------------------
//  Function: decode_lef_event
//      Decode function for event from LEF connected to usbL0 box
//
//  Inputs:
//      mode ----- USB box mode
//      info ----- Test info to be added as tag
//      fname ---- File name or NULL to use file number
//
//  Return:
//      None
//----------------------------------------------------------
void lef_raw_calib(const int mode, const char *info, const char *fname)
{
  char names[1000];
  char field[100];
  int v, c, i;

  printf("%d events\n", nevents);
  
  // Process only when has enough event
  if ( nevents < 1000 ) return;

  printf("Processing LEF RAW calib: %d events\n", nevents);
  
  // Make name prefix (once)
  if ( strlen(prename) < 10 ) {
    i = snprintf(prename, sizeof(prename), "raw_calib,name=calibration,usbbox=%s,", usbbox_names[mode]);
    if ( info != NULL )
      i += snprintf(&prename[i], (sizeof(prename) - i), "test=%s,", info);
    if ( fname == NULL )
      i += snprintf(&prename[i], (sizeof(prename) - i), "file=%04d/%03d", (last_file / 1000), (last_file % 1000));
    else
      i += snprintf(&prename[i], (sizeof(prename) - i), "file=%s", fname);
  }

  // Add file number to DB
  sprintf(names, "%s,node=%d,node_name=%s", prename, last_node, node_names[last_node]);
  if ( fname == NULL )
    snprintf(field, sizeof(field), "fileno=\"%04d/%03d\",nevents=%di", (last_file / 1000), (last_file % 1000), nevents);
  else
    snprintf(field, sizeof(field), "fileno=\"%s\",nevents=%di", fname, nevents);
  //    db_client_data(client, names, field, event_time, NO_DEBUG);

  // Clear data
  for ( c = 0; c < NCH; c++ ) {
    for ( i = 0; i < 4; i++ ) results[i][c] = 0.0;
  }

  // Calculate pedestal & raw sigma
  for ( c = 0; c < NCH; c++ ) {
    // Loop on events for pedestal
    for ( i = 0; i < nevents; i++ ) {
      results[R_PED][c] += raw_data[c][i];
    }
    results[R_PED][c] /= nevents;

    // Loop on events for raw sigma
    for ( i = 0; i < nevents; i++ ) {
      results[R_RSIG][c] += (raw_data[c][i] - results[R_PED][c]) * (raw_data[c][i] - results[R_PED][c]);
    }
    results[R_RSIG][c] = sqrt(results[R_RSIG][c] / nevents);
  }

  // Calculate common noise
  int processed[2][NCH];
  memset((void *) processed, 0, sizeof(processed));
  for ( i = 0; i < nevents; i++ ) {
    // Common noise per VA per event
    double noise[NVA];
    for ( v = 0; v < NVA; v++ ) noise[v] = NAN;

    // Loop on channels
    for ( c = 0; c < NCH; c++ ) {
      double vv;

      // VA number
      v = (c / NVACH);

      // Check common noise calculated for this va
      if ( noise[v] != noise[v] ) {
	// Loop on channels of this VA for common noise
	double sort_data[NVACH];
	int nc, vc;
	for ( nc = 0, vc = 0; vc < NVACH; vc++ ) {
	  int ch = (v * NVACH + vc);
	  vv = (raw_data[ch][i] - results[R_PED][ch]);
	  if ( fabs(vv / results[R_RSIG][ch]) < 3 ) sort_data[nc++] = vv;
	  //else printf("event=%d, v=%d, vc=%d, diff=%f, rsig=%f, diff/rsig=%f\n",
	  //            i, v, vc, vv, results[R_RSIG][ch], fabs(vv / results[R_RSIG][ch]));
	}

	//if ( i == 43 ) printf("event=%d, nc=%d\n", i, nc);
	// Sort the differences
	if ( nc >= 2 ) {
	  qsort(sort_data, nc, sizeof(double), __compare);
	  noise[v] = (0.5 * sort_data[nc / 2 - 1]) + sort_data[nc / 2];
	  //if ( i == 0 ) printf("event=%d, v=%d, nc=%d, noise=%f\n", i, v, nc, noise[v]);
	}
	//else printf("event=%d, v=%d, nc=%d\n", i, v, nc);
      }

      if ( noise[v] != noise[v] || fabs(noise[v]) > 10 ) continue;

      // Sigma and average signals
      vv = (raw_data[c][i] - results[R_PED][c] - noise[v]);
      results[R_SIG][c]  += (vv * vv);
      (processed[0][c])++;

      if ( vv > 3.5 ) {
	results[R_SIGN][c] += vv;
	(processed[1][c])++;
      }
    }
  }

  // Sigma calculation
  for ( c = 0; c < NCH; c++ ) {
    results[R_SIG][c]   = sqrt(results[R_SIG][c] / processed[0][c]);
    results[R_SIGN][c] /= processed[1][c];
    printf("c=%-4d ped=%f, rsig=%f, sig=%f, average=%f, n=%d/%d\n", c,
	   results[R_PED][c], results[R_RSIG][c], results[R_SIG][c], results[R_SIGN][c], processed[0][c], processed[1][c]);
  }

  // Write to DB
  for ( c = 0; c < NCH; c++ ) {
    // Channel and VAs
    sprintf(names, "%s,node=%d,node_name=%s,va=%d,channel=%d,va_chan=%d,adc=%d",
	    prename, last_node, node_names[last_node], (c / NVACH), c, (c % NVACH), (c % NADC));
    sprintf(field, "pedestal=%f,sigma=%f,raw_sigma=%f,signal=%f",
	    results[R_PED][c], results[R_SIG][c], results[R_RSIG][c], results[R_SIGN][c]);
    //        db_client_data(client, names, field, event_time, NO_DEBUG);
  }

  // Prepare for the next calibration
  nevents = 0;
}

//----------------------------------------------------------
//  Function: decode_lef_event
//      Decode function for event from LEF connected to usbL0 box
//
//  Inputs:
//      block ---- Data words (16-bit)
//      utime ---- Time tag for DB entry
//      mode ----- USB box mode
//      info ----- Test info to be added as tag
//
//  Return:
//      None
//----------------------------------------------------------
void decode_lef_raw_event(AMSBlock *block, uint64_t utime, const int mode, const char *info)
{
  int c, b, i, a;

  printf("file_no=%d, last_file=%d\n", file_no, last_file);
  
  // Check file no (do one calibration with one data file)
  if ( last_file < 0 ) last_file = file_no;
  else if ( last_file != file_no ) {
    // make calibration
    lef_raw_calib(mode, info, NULL);//MD: essentially one calib for ech one minute block file... To be changed
    
    // Save file number
    last_file = file_no;
  }
  
  // Skip
  if ( block == NULL ) return;
  int s = block->DataCount;
  if ( s != 1794 ) return;//not good size
  
  // Check trigger number
  uint8_t *p = abiDataPtr8(block);
  //     if ( trig_numb >= 0 && (trig_numb & 0xFF) == p[1] ) return;
  //     trig_numb = p[1];
  //     if ( info != NULL )
  //         sprintf(names, "raw_events,test=%s,file=%d,name=trig_number,node=%d,node_name=%s",
  //                 info, file_no, block->NodeAddress, node_names[block->NodeAddress]);
  //     else
  //         sprintf(names, "raw_events,file=%d,name=event_number,node=%d,node_name=%s",
  //                 file_no, block->NodeAddress, node_names[block->NodeAddress]);
  //     sprintf(field, "trig_numb=%di", trig_numb);
  //     db_client_data(client, names, field, utime, NO_DEBUG);
  
  // Decode event data
  for ( p += 2, i = 0; i < (NCH / NADC); i++, p += 14 ) {
    // Loop on ADCs
    for ( a = 0; a < NADC; a++ ) {
      // Init data
      uint16_t w = 0;

      // Loop on 14 bits
      for ( b = 0; b < 14; b++ ) {
	w |= ((p[b] >> a) & 1) << b;
      }

      // one ADC word finished
      b = (a * (NCH / NADC) + i);                             // Original channel number
      c = (((NVA - 1) - (b / NVACH)) * NVACH + (b % NVACH));  // Corrected channel number

      // Save data
      if ( nevents < LEF_RAW_MAX ) raw_data[c][nevents] = ((double) w);

      //             // Difference from pedestal
      //             if ( info != NULL )
      //                 sprintf(names, "raw_events,test=%s,file=%d,name=data,va=%d,channel=%d,node=%d,node_name=%s,adc=%d",
      //                         info, last_file, (15 - v), ((15 - v) * 64 + c), block->NodeAddress, node_names[block->NodeAddress], a);
      //             else
      //                 sprintf(names, "raw_events,file=%d,name=data,va=%d,channel=%d,node=%d,node_name=%s,adc=%d",
      //                         last_file, (15 - v), ((15 - v) * 64 + c), block->NodeAddress, node_names[block->NodeAddress], a);
      //             if ( ped_saved == 0 )
      //                 sprintf(field, "raw=%f", d);
      //             else
      //                 sprintf(field, "raw=%f,diff=%f", d, (d - pedestal[15 - v][c]));
      //             db_client_data(client, names, field, event_time, NO_DEBUG);
    }
  }

  // Save last event time
  if ( nevents < LEF_RAW_MAX ) {
    nevents++;
    event_time = utime;
    last_node  = block->NodeAddress;
  }
}

//----------------------------------------------------------
//  Function: decode_ground_test
//      Decode function for Ground Test Data
//
//  Inputs:
//      block ---- AMS block
//      utime ---- Time in microseconds
//      mode ----- USB box mode
//      info ----- test info to be added as tag
//
//  Return:
//      None
//----------------------------------------------------------
void decode_l0_upgrade(AMSBlock *block, uint64_t utime, const int mode, const char *info)
{

  printf("DT: 0x%x\n", block->DataType);
  
  // Datatypes
  switch (block->DataType) {
    // USB box read event reply
  case 0x13:
    decode_lef_raw_event(block, utime, mode, info);
    break;
  }
}

//----------------------------------------------------------
//  Function: process_block
//      Prints one single block or one sub-block in envelop
//
//  Inputs:
//      caller_data ---- Caller data
//      pBlock --------- AMS Block to be printed
//
//  Return:
//      true  : Success
//      false : Failed
//----------------------------------------------------------
static bool process_block(void *caller_data, AMSBlock *block)
{
  char name[100], field[100];
    
  // No block
  if ( block == NULL ) return FALSE;
    
  // Debug message
  if ( caller_data != NULL && debug_output > 1 ) abiPrint("--Sub", block, 0);

  /*
  // Skip blocks are not a read reply
  if ( block->BlockType != (BT_REPLY | BT_WRITEREAD) ) goto __end_of_process;
  */
    
  // prepare the time
  _utime = ((uint64_t) block->DataTime * 1000000 + block->FineTime);
    
  /*
  // Block error
  if ( block->Error != 0 ) {
  // Check only by HKBPB stream
  if ( check_dsp_err == 0 ) goto __end_of_process;

  //         sprintf(name, "BlockErrors,node=%s,address=%d", node_names[block->NodeAddress], block->NodeAddress);
  //         sprintf(field, "error=true,error_code=%d", block->Error);
  //         db_client_data(client, name, field, ((uint64_t) block->DataTime * 1000000), NO_DEBUG);

  // Find error list (using node address as tags only)
  state_var_t *es = state_list_find(block_errs, NULL, node_names[block->NodeAddress]);
  if ( es == NULL ) {
  es = state_var_new(0, "BlockErrors,node=NErrors,address=512",
  "nerrors", node_names[block->NodeAddress], "error");     // Use node address
  if ( es != NULL ) state_list_add(&block_errs, es);
  }

  if ( es == NULL || es->state != block->Error || _utime > (es->time + 10 * 1000000) ) {
  sprintf(name, "BlockErrors,node=%s,address=%d", node_names[block->NodeAddress], block->NodeAddress);
  sprintf(field, "error=1i,error_code=%di", block->Error);
  db_client_data(client, name, field, _utime, NO_DEBUG);

  // Save current state
  if ( es != NULL ) {
  es->state = block->Error;
  es->time  = _utime;
  }
  }
  goto __end_of_process;
  }

  // No error state (period of every 30 min.)
  if ( block_errs == NULL ) {
  // Only check from one stream (default is HKBPB)
  if ( check_dsp_err != 0 )
  state_var_set(&blk_nerrs, 0, sINT, _utime, s30MIN, NO_DEBUG);
  }

  // Check error state
  else {
  state_var_t *es = state_list_find(block_errs, NULL, node_names[block->NodeAddress]);
  if ( es != NULL ) {
  // No more error
  if ( es == block_errs && es->next == NULL ) state_var_set(es, 0, sINT, _utime, 0, NO_DEBUG);

  // Remove the error state from the list
  state_list_del(&block_errs, es);
  }
  }

  // Save block time for using in other routines
  strftime(block_time, sizeof(block_time), "%Y-%m-%d(%j)/%H:%M:%S", gmtime((time_t *) &(block->DataTime)));

  // Envelope
  if ( block->DataType == 0x1F0380 && block->NodeAddress < 21 ) {
  abiProcessEnvelope(process_block, (void *) 1, block);
  //         abiProcessEnvelope2(process_block, (void *) 1, block, TRUE);
  }

  // Check envelope for fine time
  else if ( block->DataType == 0x1F0383 && block->NodeAddress < 21 ) {
  // Get fine time
  uint8_t *ptr8 = abiDataPtr8(block);
  uint32_t ft   = bswap_32(*((uint32_t *) ptr8));
  ptr8 += 4;

  // Get size
  uint32_t size = abiSizeWord(&ptr8);

  // Decode block
  AMSBlock *b = abiDecode(ptr8, size, TRUE, FALSE);

  // Process subblock
  if ( b != NULL ) {
  // Add fine time
  b->FineTime = ft;

  // Process
  process_block(((void *) 1), b);
  }
  }

  // AMSWire group commands
  else if ( block->NodeAddress >= TDR_0_M && block->NodeAddress <= TDR_D ) {
  process_amsw_gcmd(process_block, (void *) 1, block);
  }
  */

  if (block->DataType != 0x1f0383) printf("NA: %x, DT: %x\n", block->NodeAddress, block->DataType);
    
  // All single blocks
  //    else {
  {
    // Node address classify
    switch ( block->NodeAddress ) {
      // JMDC
    case JMDC_0_CAN ... JMDC_SELF:
    case JMDC_ALL:
      decode_jmdc(block, _utime);
      break;
      /*
      // Laptop
      case ALC_HRDL:
      case ALC_RIC:
      decode_alc(block, _utime);
      break;

      // CAN
      case CAB_A ... CCEB_S:
      case JPD_A ... PDS_S:
      case UG_A ... UG_S:
      decode_can_nodes(block, _utime);
      break;

      // TTCS
      case TTCE_A ... TTCE_S:
      decode_ttcs_nodes(block, _utime);
      break;

      // UTTPS
      case UTT_TTGA_A ... UTT_TTPC_P:
      decode_uttps(block, _utime);
      break;

      // DSP
      case JINJ_0 ... ETRG_1_S:
      case JINF_E_0_A ... UDR_1_5_P:
      decode_dsp(block, _utime);
      break;

      // UTTPS Pump Test Data (using usbTTTC, usbTTPC and usbGSE boxes)
      case USB_BOX_0 ... USB_BOX_3:
      decode_pumptest(block, _utime, usbbox_mode);
      break;
      */
      // L0 Upgrade
    case LINJ_A ... LINF_U_3_P:
      decode_l0_upgrade(block, _utime, usbbox_mode, test_info);
      break;
      /*
      // Ground Test Node
      case GRD_TEST:
      decode_ground_test(block, _utime);
      break;
      */

      // FEPs
    case FEP_NODE:
      decode_fep(block, _utime);
      break;

      // Unknown node addresses
    default:
      break;
    }
  }

 __end_of_process:
  // Deconstruct of the block
  abiDestruct(block);
  return TRUE;
}

void OpenAMSL0FEPFile(TString filename, std::vector<TH1F*>& histos){
  
  char fname[256] = "";
  sprintf(fname, "%s", filename.Data());

  printf("***Assegnare file_no al file corrente!***\n");
  sleep(10);
  
  // File open
  FILE* file = fopen(fname, "rb");

  AMSBlock *block = NULL;

  printf("Qui0!\n");
  
  while (1) {
    block = abiFileRead(file, fname, TRUE);
    if (block == NULL) break;
    
    // Process block
    //  if ( debug_output > 1 ) abiPrint("BLOCK", block, 0);
    process_block((void *) 0, block);
    block = NULL;
  }// all events were read and accumulated

  printf("Qui!\n");
  
  lef_raw_calib(usbbox_mode, test_info, NULL);

  printf("Qui2!\n");
  
  /*
  const char* names[4] = {"Pedestals", "Sigma", "RawSigma", "AverageSignal"};
  
  std::vector<TH1F*> histos2;
  
  for (int ii=0; ii<4; ii++) {
    std::pair<TH1F*, TH1F*> coppia = VectorToHisto(values[ii], names[ii], filename.Data(), nch);
    TH1F* h = coppia.first;
    TH1F* h2 = coppia.second;
    printf("%s %s %p %p\n", "AMS-L0", names[ii], h, h2);
    // h->Dump();
    // for (int bb=0; bb<=h->GetNbinsX()+1; bb++){
    // printf("%d) %f\n", bb, h->GetBinContent(bb));
    // }
    histos.push_back(h);
    histos2.push_back(h2);
  }
  
  for (int ii=0; ii<3; ii++) {//not including the spread on the average signal
    histos.push_back(histos2.at(ii));
  }
  */

  return;
}
