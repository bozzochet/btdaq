
#include "DeBlock.h"
#include "POutF.h"

#include "tkdcards.h"
#include "TrRecon.h"
#include "TrCalDB.h"
#include "TrField.h"
#include "VCon_root.h"
#include "timeid.h"

#include "TFile.h"
#include "TTree.h"
#include "TString.h"
#include "TStopwatch.h"

#include <iostream>
#include <fstream>

TString blkdir = "sci";
TString outdir = ".";
TString dbname = "";

int  runnum = 0;
int  maxev  = 999999999;
int  tree   = 2;
int  reco   = 1;
int  skipev = 0;
int verbose = 0;

int procargs(int argc, char *argv[]);

void  ffkeys();
int   decode(DeBlock &dblock);
int   getcal(time_t run);
float memchk(void);

int main(int argc, char ** argv)
{ 
  // Process command line input arguments
  if (procargs(argc, argv) < 0) {
    cerr << "Error: from the command line" << endl;
    return 1;
  }

  // Create and Init the Tracker Database
  TkDBc::CreateTkDBc();
  if (dbname == "") TkDBc::Head->init();
  else TkDBc::Head->init(dbname);

  // Get TrCalDB
  if (!getcal(runnum) || !TrCalDB::Head) {
    cerr << "TrCalDB not found" << endl;
    return -1;
  }

  // Create the reconstruction object
  TrRecon::Create();

  // Set TrCalDB pointer
  TrClusterR   ::UsingTrCalDB(TrCalDB::Head);
  TrRawClusterR::UsingTrCalDB(TrCalDB::Head);
  TrRecon      ::UsingTrCalDB(TrCalDB::Head);

  // Set up reconstruction parameters in FFKEYS
  ffkeys();
  
  // Create Event based container
  TrRecon::TRCon = new VCon_root();

  // Create DeBlock
  new POutF();
  DeBlock dblock;

  // Create an event object
  Event *event = dblock.ev = new Event;

  // Open the output root file and create TTree
  TFile *rofile = 0;
  TTree *t4tree = 0;
  if (tree){
    rofile = new TFile(outdir+Form("/data_%d.root", runnum), "RECREATE");
    TTree::SetMaxTreeSize(10000000000LL); // 10 GB

    t4tree = new TTree("t4","AMSEvents");
    t4tree->Branch("Tracker","Event", &event);
    TrCalDB::Head->Write();
  }

  // Open raw data file
  TString fullfname = blkdir+Form("/%d", runnum);
#ifdef __LINUXGNU__
  FILE* filein = fopen64(fullfname, "r");
#else  
  FILE* filein = fopen(fullfname, "r");
#endif 
  if (!filein) {
    cerr << "FATAL. Problems opening: " << fullfname.Data() << endl;
    return 2;
  }
  dblock.SetInputFile(filein);

  TStopwatch timer;
  timer.Start();

  int count = -1, nfill = 0, ntrk = 0, intv = 1;

  while (!feof(dblock.rawfile) && ++count < maxev) {
    if (count%intv == 0 && count > 0) {
      Double_t tm = timer.RealTime(); timer.Continue();
      Double_t rt = (tm > 1 && count > 0) ? (double)count/tm/1000 : 0;
      cout << Form("Event: %7d  Fill: %6d %7d  "
		   "%4.0f sec (%4.2f kHz) Mem: %4.1f MB",
		   count, ntrk, nfill, tm, rt, memchk()/1024) << endl;
      if (intv < 1000) intv *= 10;
    }

    int ret = decode(dblock);
    if (ret < 0) break;
    if (!ret) continue;
    if (count < skipev) return 0;

    // Build TrClusters, TrRecHits, and TrTracks
    int rret = TrRecon::gethead()->Build(reco);
    if (event->NTrTracks > 0) ntrk++;
    if (tree == 1 || (tree == 2 && rret)){
      t4tree->Fill();
      nfill++;
    }
  }

  fclose(filein);
  cout << Form("Event: %7d  Fill: %6d %7d  %4.0f sec", 
	       count-skipev, ntrk, nfill, timer.RealTime()) << endl;

  if(tree && rofile && t4tree) {
    t4tree->Write("", TObject::kOverwrite);
    rofile->Write();
    delete rofile;
  }
  
  return 0;
}

#include <unistd.h>
#include <getopt.h>

void usage(char *name)
{
  cout << Form("Usage:\t %s [options] -r runnum", name) << endl;
  cout << "Options:" << endl;
  cout << "\t-h -? --help            \tprint this message" << endl;
  cout << "\t-M    --maxev <n>       \tmax. number of events to be decoded."
       << Form("[%d]", maxev) << endl;
  cout << "\t-S    --skipev <n>      \tnumber of events to be skipevped."
       << Form("[%d]", skipev) << endl;
  cout << "\t-B    --blockdir <path> \tdirectory for blocks."
       << Form("[%s]",blkdir.Data()) << endl;
  cout << "\t-O    --outputdir <path>\tdirectory for output file."
       << Form("[%s]",outdir.Data()) << endl;
  cout << "\t-T    --tree   <opt>    \tOutput tree option.[2]" << endl;
  cout << "\t-R    --reco   <opt>    \tSet reco. option.[1]" << endl;
  cout << "\t-D    --tkdbc  <fname>  \tSpecify TkDBc file" << endl;
}

int procargs(int argc, char *argv[]) 
{
  static struct option long_options[] = {
    {"help",      0, 0, 'h'},
    {"Run",       1, 0, 'r'},
    {"maxev",     1, 0, 'M'},
    {"skipev",    1, 0, 'S'},
    {"blockdir",  1, 0, 'B'},
    {"outputdir", 1, 0, 'O'},
    {"tree",      1, 0, 'T'},
    {"tkdbc",     1, 0, 'D'},
    {"reco",      1, 0, 'R'},
    {0, 0, 0, 0}
  };

  if (argc == 1) {
    usage(argv[0]);
    return 0;
  }
  
  int copt, option_index = 0;
  while((copt = getopt_long(argc, argv, "hH:T:R:D:r:O:iS:M:B:?", 
			    long_options, &option_index)) > 0) {
    switch (copt) {
    case 'r': runnum = atoi(optarg); break;
    case 'M': maxev  = atoi(optarg); break;
    case 'S': skipev = atoi(optarg); break;
    case 'T': tree   = atoi(optarg); break;
    case 'R': reco   = atoi(optarg); break;
    case 'B': blkdir = optarg; break;
    case 'O': outdir = optarg; break;
    case 'D': dbname = optarg; break;
    case 'H':
    case 'h':
    case '?':
    default:
      usage(argv[0]);
      break;
    }
  }

  return 0;
}

void ffkeys()
{
  TKGEOMFFKEY.init();
  TRMCFFKEY  .init();
  TRCALIB    .init();
  TRALIG     .init();
  TRCLFFKEY  .init();
  TRFITFFKEY .init();

  TRFITFFKEY.ErrX = 300e-4;
  TRFITFFKEY.ErrY = 300e-4;

  MAGFIELDFFKEY.magstat = 0;

  TrRecon::gethead()->SetParFromDataCards();

  TrRecon::TrDEBUG   = 0;
  TrRecon::MaxNtrack = 2;            // Default 2
  TrRecon::MinNhitX  = 4;            // Default 5
  TrRecon::MinNhitY  = 5;            // Default 6
  TrRecon::MinNhitXY = 4;            // Default
  TrRecon::MaxChisqAllowed   = 300;  // Default
  TrRecon::LadderScanRange   = 7.3;  // Default
  TrRecon::ClusterScanRange  = 0.5;  // Default
  TrRecon::MaxChisqForLScan  = 2.2;  // Default
  TrRecon::PatAllowOption    = 0;    // Default
}

int decode(DeBlock &dblock)
{
  static int prev_time = 0, prev_ftime = 0;
  static double prev_fftime = 0.;

  POutF::pp->Clear();
  dblock.Clear();
  for (int kk = 0; kk <  24; kk++) dblock.frag_size[kk] = -1;
  for (int kk = 0; kk < 192; kk++) dblock.tdr_size [kk] = -1;

  dblock.ev->Clear(1);
  if (dblock.ReadOneBlock() < 0) return -1;

  dblock.DecodeBlockHeader();
  dblock.DecodeJMDCSubBlock();

  if(dblock.JMDC.runnum>=1210067687) dblock.TDROff=2;
  if(dblock.JMDC.runnum>=1208965124) dblock.SetCNWords(0);
  if(dblock.JMDC.runnum>=1210237802) dblock.TDROff=1;
  if(dblock.JMDC.runnum==1210262957) dblock.TDROff=2;
  if(dblock.JMDC.runnum>=1210668275 &&
     dblock.JMDC.runnum< 1210678767) dblock.TDROff=2; 

  dblock.ev->runnum        = dblock.JMDC.runnum;
  dblock.ev->runtag        = dblock.JMDC.runtag;
  dblock.ev->eventnum      = dblock.JMDC.eventnum;
  dblock.ev->event_time    = dblock.JMDC.event_time;
  dblock.ev->event_ftime   = dblock.JMDC.event_ftime;
  dblock.ev->event_fftime  = dblock.JMDC.event_fftime;
  dblock.ev->event_size    = dblock.block_size2;
  dblock.ev->event_timeP   = prev_time;
  dblock.ev->event_ftimeP  = prev_ftime;
  dblock.ev->event_fftimeP = prev_fftime;
  prev_time   = dblock.ev->event_time;
  prev_ftime  = dblock.ev->event_ftime;
  prev_fftime = dblock.ev->event_fftime;

  if (dblock.ScanBlocks(0) < 0) return 0;

  dblock.ev->TofFlag1     = dblock.JMDC.TofFlag1;
  dblock.ev->event_fftime = dblock.JMDC.event_fftime;

  return 1;
}

int getcal(time_t run)
{
  TrCalDB* cc = new TrCalDB();
  cc->init();
  cc->CreateLinear();

  time_t bb = 0;
  tm begin;
  tm end;
  tm* mtim=localtime_r(&bb,&begin);
  
  begin.tm_isdst = begin.tm_sec = begin.tm_min  = begin.tm_hour = 0;
  begin.tm_mday  = begin.tm_mon = begin.tm_year = 0;
  end.tm_isdst = end.tm_sec = end.tm_min = end.tm_hour = 0;
  end.tm_mday  = end.tm_mon = end.tm_year = 0;

  AMSTimeID *tt = new AMSTimeID(AMSID("TrackerCals", 1), begin, end,
				TrCalDB::GetLinearSize(),
				TrCalDB::linear,AMSTimeID::Standalone,
				1, SLin2CalDB);
  tt->validate(run);
  time_t in, bg, en;
  tt->gettime(in, bg, en);  
  tm pout;

  return 1;
}

float memchk(void)
{
  std::ifstream fin(Form("/proc/%d/status", getpid()));
  if (!fin) return 0;

  TString sbuf;
  while (!sbuf.ReadToken(fin).eof()) {
    if (sbuf == "VmSize:") {
      float vmsize;
      fin >> vmsize;
      return vmsize;
    }
  }
  return 0;
}
