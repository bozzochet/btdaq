
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include "TrRecon.h"
#include "TrCalDB.h"
#include "TFile.h"
#include "TTree.h"
#include "DeBlock.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <getopt.h>
#include "TrDHF.h"
#include "POutF.h"
#include "TProcessID.h"
#include "myholder.h"
#include "tkdcards.h"
#include "VCon_root.h"
#include "timeid.h"

typedef  unsigned short int ushort;
int goodrun=0;
char block_dir[320];
char outdir[320];
char calfname[120];
char caldir[120];
char dbcname[120] = "";
int maxev=999999999;
int verbose=0;  
int histo=0;
int tree=1;
int reco=1;
int skip=0;
int testerr=0;
float memchk(void);

void  LFillAll(Event* ev,TrDHF* bb,int flag);
void  LBookHistos(TrDHF* bb,int flag);
void GetTheRightCal(time_t run);


extern  char label[25][10];

void _usage(char * nn);
void _usage(char *name) {
  printf("Usage:\t %s [options] -r runnum  [-C calfname]\n",name);
  printf("Options:\n");
  printf("\t-h -? --help              \tprint this message\n");
  printf("\t-M    --maxev <number>   \tthe maximum number of events to be decoded. Default is %d\n",maxev);
  printf("\t-S    --skip  <number>   \tthe number of events to be skipped. Default is %d\n",skip);
  printf("\t-B    --blockdir <path>   \tthe dir where to search for blocks. Default is %s \n",block_dir);
  printf("\t-O    --outputdir <path>  \tthe dir where to put the output files. Default is %s \n",outdir);
  printf("\t-G    --caldir <path>  \tthe dir where to search for calibrations files. Default is %s \n",caldir);
  printf("\t-C    --calib <calrun>  \tthe run to be used for calibration. Default it search for CalDB_prima.root \n");
  printf("\t-V    --verbose <level>  \tverbose level (0: quiet, 1: report errors 2: full), default 0 \n");
  printf("\t-H    --histos <0/1> def 0 \tOutput Monitor Histos, default 0 \n");
  printf("\t-T    --tree   <0/1> def 1 \tOutput The decoded root file, default 1\n");
  printf("\t-R    --reco   <1/2> def 1 \tReconstruction mode 1:Only clusters 2:Up to track\n");
  printf("\t-E    --testerr def notset \tTest Error mode. Imply (if not set after) -T0 -H0 -V1 -S0. \n");
  printf("\t-D    --tkdbc  <tkdbc fname> \tSpecify TkDBc file\n");
  //  printf("\t-N    --nocnoise def notset  \tEnable the new data format without the CN words\n");
  exit(0);
}


int _proc_args(int argc, char *argv[]);

//----------------------------------------------------------
int _proc_args(int argc, char *argv[]) {
  int c;
  int option_index = 0;
  static struct option long_options[] = {
    {"help",     0, 0, 'h'},
    {"testerr",    0, 0, 'E'},
    {"maxev",      1, 0, 'M'},
    {"skip",      1, 0, 'S'},
    {"Run",      1, 0, 'r'},
    {"caldir",    1, 0, 'G'},
    {"calib",    1, 0, 'C'},
    {"blockdir", 1, 0, 'B'},
    {"outputdir", 1, 0,'O'},
    {"histos", 1, 0,'H'},
    {"tree", 1, 0,'T'},
    {"reco", 1, 0,'R'},
    {"tkdbc", 1, 0,'D'},
    //    {"nocnoise", 0, 0,'N'},
    {0, 0, 0, 0}
  };

  /* Turn off error check for getopt */
  opterr = 0;

  /* Loop on options */
  while (1) {
    c = getopt_long (argc, argv, "hEH:T:G:R:D:r:O:iS:M:B:C:V:?", long_options, &option_index);
    if (c == -1) break;

    switch (c) {

    case 'r':             /* logdir */
      goodrun=atoi(optarg);
      break;
    case 'M':             /* logdir */
      maxev=atoi(optarg);
      break;
    case 'S':             /* logdir */
      skip=atoi(optarg);
      break;
    case 'C':
      sprintf(calfname,"%s",optarg);
      break;
    case 'B':
      sprintf(block_dir,"%s",optarg);
      break;
    case 'O':
      sprintf(outdir,"%s",optarg);
      break;
    case 'G':
      sprintf(caldir,"%s",optarg);
      break;
    case 'V':
      verbose=atoi(optarg);
      break;
    case 'T':
      tree=atoi(optarg);
      break;
    case 'H':
      histo=atoi(optarg);
      break;
    case 'R':
      reco=atoi(optarg);
      break;
    case 'D':
      sprintf(dbcname,"%s",optarg);
      break;
    case 'E':
      testerr=1;
      histo=1;
      tree=0;
      verbose=1;
      reco=0;
      break;
    case '?':
    default:            /* help */
      _usage(argv[0]);
      break;
    }
  }

  /* No input dir/file from command line */
  if ( argc == 1 ) _usage(argv[0]);
  /* Check path from command line */
  return 0;

}


AMSContainer * GetCEvent(AMSID id);
// AMSContainer * GetCEvent(AMSID id){
//   return
//     AMSEvent::gethead()->getC(id);
	          
// }

int main(int argc, char ** argv){ 

  
  //  TrReconstruction* trec = new TrReconstruction();
  char calibname[120]; 
  char fullfname[120]; 
  char foutname[120];
  int istat;
  TrDHF* Histos=0;

  sprintf(outdir,"./");
  sprintf(block_dir,"./runs");  
  sprintf(calfname,"prima");
  sprintf(caldir,"./");

  //   /* Process command line input arguments */
  istat = _proc_args(argc, argv);

  //   /* Error from command line */
  if ( istat < 0 ) {
    printf("Error: from the command line\n");
    exit(1);
  }

  // Create and Init the Tracker Database
  TkDBc::CreateTkDBc();
  if (dbcname[0] == 0) TkDBc::Head->init();
  else TkDBc::Head->init(dbcname);



  //init the datacards
  TKGEOMFFKEY.init();
  TRMCFFKEY.init();
  TRCALIB.init();
  TRALIG.init();
  TRCLFFKEY.init();
  TRFITFFKEY.init();
//   //int readcalibration(char *fname)
//   sprintf(calibname,"%s/CalDB_%s.root",caldir,calfname); 
//   struct stat buf;
//   if(stat(calibname, &buf)!=0){
//     perror("Error opening the Calibration file");
//     exit(2);
//   }
//   TrCalDB::Load(calibname);

  GetTheRightCal(goodrun);

  TrClusterR::UsingTrCalDB(TrCalDB::Head);
  TrRawClusterR::UsingTrCalDB(TrCalDB::Head);


  // For Vitaly's Container structure
  //myholder *hold=new myholder();

 
  // Create the reconstruction object
  TrRecon::Create();
  TrRecon::UsingTrCalDB(TrCalDB::Head);
  
  TrRecon::gethead()->SetParFromDataCards();
 //  TrRecon::gethead()->ThrSeed[0]=4.2;
//   TrRecon::gethead()->ThrSeed[1]=4.2;

//   TrRecon::gethead()->ThrNeig[0]=1.5;
//   TrRecon::gethead()->ThrNeig[1]=1.5;
  
  TrRecon::TRCon= new VCon_root();
  //The print out facility
  new POutF();

  TH2F* thsiz;
  TH2F* thsiz2;
  TProfile* thsiz3;
  TH2F* frsiz;
  TH2F* tdsiz;
  TH2F* tcsiz[25];
  TH2F* mtcsiz[192];
  TH2F* tdrsiz[25];
  DeBlock dblock;
  dblock.ev= new Event();
  if(testerr) dblock.SetOnlyTestErrors();
  sprintf(foutname,"%s/data_%d.root",outdir,goodrun);
  TTree* t4=0;
  TFile *f=0;
  TFile *gg=0;

  if(tree){
    // Open the output root file
    f= new TFile(foutname,"RECREATE");
    //    TTree::SetMaxTreeSize(2900000000);
    t4= new TTree("t4","AMSEvents");
    t4->Branch("Tracker","Event",&(dblock.ev));
    TrCalDB::Head->Write();
    f->Append(dblock.hh1);
  }
  if(histo){
    char name[200];
    char tname[200];
    char ttitle[200];
    //File di uscita
    sprintf(name,"Houtdst_%d.root",goodrun);   
    gg=TFile::Open(name,"recreate");
    //  TrDHF* Histos= new TrDHF((TDirectory*)gg,"HISTOS","HISTOS");

    Histos= new TrDHF(0,"HISTOS","HISTOS");
    LBookHistos(Histos,0);
    TDirectory* gg2=gg->mkdir("Sizes","Sizes");
    gg2->cd();
    thsiz2= new TH2F("gg2","gg2",1000,0,10000,150,0,15000);
    thsiz= new TH2F("TotSizVsDeltaT","TotSizVsDeltaT;Delta T (musec); Size (words)",2000,-10000,10000,150,0,15000);
    thsiz3= new TProfile("TotSizVsDeltaTP","TotSizVsDeltaT;Delta T (musec); Size (words)",2000,0,1000);
    frsiz= new TH2F("frsiz","gg" , 25,0, 25,15000,0,15000);
    tdsiz= new TH2F("tdrsiz","gg",192,0,192,1500,0,1500);
    for (int kk=0;kk<25;kk++){
      frsiz->GetXaxis()->SetBinLabel(kk+1,label[kk]);
      sprintf(tname,"%sSizVsDeltaT",label[kk]);
      sprintf(ttitle,"%sSizVsDeltaT;Delta T ($\\mu$sec); Size (words)",label[kk]);
      tcsiz[kk]= new TH2F(tname,tname,200,-1000,1000,1400,0,14000);
      //      sprintf(tname,"TDR%02dSizVsDeltaT",kk);
      //      sprintf(ttitle,"TDR%02dSizVsDeltaT;Delta T ($\\mu$sec); Size (words)",kk);
      //      tdrsiz[kk]= new TH2F(tname,tname,90,1.5,4,160,0,4);
    }
    tdsiz= new TH2F("tdrsiz","gg",192,0,192,1500,0,1500);
    for (int kk=0;kk<192;kk++){
      sprintf(tname,"Siz_lad%03dVsDeltaT",kk+1);
      sprintf(ttitle,"Size Lad %03d Vs DeltaT;Delta T ($\\mu$sec); Size (words)",kk+1);
      mtcsiz[kk]= new TH2F(tname,tname,200,-1000,1000,5000,0,5000);
      //      sprintf(tname,"TDR%02dSizVsDeltaT",kk);
      //      sprintf(ttitle,"TDR%02dSizVsDeltaT;Delta T ($\\mu$sec); Size (words)",kk);
      //      tdrsiz[kk]= new TH2F(tname,tname,90,1.5,4,160,0,4);
    }
    gg->cd();
  }


  sprintf(fullfname,"%s/%d",block_dir,goodrun);
  int aa;
  int count=-1;
  int count3=0;
  int count4=0;
#ifdef __LINUXGNU__
  FILE* filein = fopen64(fullfname,"r");
#else  
  FILE* filein = fopen(fullfname,"r");
#endif 
  if(!filein){
    printf("FATAL. Problems opening %s\n",fullfname); exit(2);
  }
  int prev_time=0;
  int prev_ftime=0;
  double prev_fftime=0.;
  int first=1;
  int evfirst=-1;
  int evlast=-1;
  //---------- SH add ----------
  int nfill=0;
  int intv=1000;
  time_t tm0, tm1;
  time(&tm0);
  
  int NrawMax = 200;
  int NclsMax = 200;
  int NhitMax = 200;

  // For SH analysis
  if (reco == 2) {
    NclsMax = 100;
    NhitMax =  40;
  }

  //   // Some settings in TrCluster
  //   trec->SetThrSeed(0, 4); trec->SetThrNeig(0, 1);
  //   trec->SetThrSeed(1, 4); trec->SetThrNeig(1, 1);
  //   trec->SetThrProb(0);

  //   // Some settings in TrTrack
  //   TrTrack::MaxChisq[0] = 100;
  //   TrTrack::MaxChisq[1] = 10000;
  //   TrTrack::SearchRegStrLine = 100;
  //   TrTrack::SearchRegCircle  = 100;
  //   TrTrack::MaxNtrack = 1;
  //   TrTrack::ErrX = 300e-4;
  //   TrTrack::ErrY = 300e-4;

  //   // Magnet is off
  //   TrField::_magstat = 0;
  //---------- SH add ----------
  int tof4=0;
  int tof3=0;
  int tof21=0;
  int tof20=0;
  int errcount=0;
  dblock.SetInputFile(filein);
  while(!feof(dblock.rawfile)){
    //while(count<10){
    count++;
    POutF::pp->Clear();
    dblock.Clear();
    for(int kk=0;kk<24;kk++)  dblock.frag_size[kk]=-1;
    for(int kk=0;kk<192;kk++) dblock.tdr_size[kk]=-1;
    if(count>maxev) break;
    
    if (verbose > 0) {
      PRINTF("-----------------------------------------------------\n");
      PRINTF("---------------- BLOCK #%d --------------------------\n ",count);
      PRINTF("-----------------------------------------------------\n");
    }
    if (count%intv == 0) {
      time(&tm1);
      cerr << Form("BLOCK #%6d  Filled %6d  Memory: %5.1f MB  "
		   "Time: %4d  Rate: %6.1f Hz",
		   count, nfill, memchk()/1024, tm1-tm0, 
		   (tm1-tm0 > 0) ? (double)count/(tm1-tm0) : 0) << endl;
    }
    

    dblock.ev->Clear(1);
    
    

    
    
    aa=dblock.ReadOneBlock();
    if(aa<0) break;
    dblock.DecodeBlockHeader();
    if (verbose > 0) dblock.bh1.Print();    
    if(dblock.bh1.type!=5)  {POutF::pp->Print(); continue;} //the type data
    if(dblock.bh1.node!=15) {POutF::pp->Print();continue;} //the right JMDC
    dblock.DecodeJMDCSubBlock();
    if(count<skip) continue;
    if (verbose > 0) dblock.JMDC.Print();
    if(dblock.JMDC.runnum>=1210067687) dblock.TDROff=2;
    if(dblock.JMDC.runnum>=1208965124) dblock.SetCNWords(0);
    if(dblock.JMDC.runnum>=1210237802) dblock.TDROff=1;
    if(dblock.JMDC.runnum==1210262957) dblock.TDROff=2;
    if(dblock.JMDC.runnum>=1210668275 &&
       dblock.JMDC.runnum< 1210678767) dblock.TDROff=2; 
    //       if(dblock.JMDC.runnum==goodrun){
    if(first){first=0;evfirst=dblock.JMDC.eventnum;}
    evlast = dblock.JMDC.eventnum;
    dblock.ev->runnum      = dblock.JMDC.runnum;
    dblock.ev->runtag      = dblock.JMDC.runtag;
    dblock.ev->eventnum    = dblock.JMDC.eventnum;
    dblock.ev->event_time  = dblock.JMDC.event_time;
    dblock.ev->event_ftime = dblock.JMDC.event_ftime;
    dblock.ev->event_fftime = dblock.JMDC.event_fftime;
    dblock.ev->event_size  = dblock.block_size2;
    dblock.ev->event_timeP  = prev_time;
    dblock.ev->event_ftimeP = prev_ftime;
    dblock.ev->event_fftimeP = prev_fftime;
    prev_time=dblock.ev->event_time;
    prev_ftime=dblock.ev->event_ftime;
    prev_fftime=dblock.ev->event_fftime;

    int bb=dblock.ScanBlocks(verbose);
    dblock.ev->TofFlag1 = dblock.JMDC.TofFlag1;
    int tcrate=1;
    dblock.ev->event_fftime = dblock.JMDC.event_fftime;

    if(dblock.ev->TofFlag1==0) { tof4++; tof3++;tof21++;tof20++;}
    else if(dblock.ev->TofFlag1>0 && dblock.ev->TofFlag1<=4) { tof3++;tof21++;tof20++;}
    else if(dblock.ev->TofFlag1>4 && dblock.ev->TofFlag1<=8) { tof21++;tof20++;}
    else if(dblock.ev->TofFlag1>8 && dblock.ev->TofFlag1<=10) { tof20++;}
    
    
  
    if(histo>0){
      int err=(bb>=0)?1:-1;
      thsiz->Fill(err*dblock.ev->event_fftime,dblock.block_size2);
      thsiz3->Fill(err*dblock.ev->event_fftime,dblock.block_size2);
      int frsum=0;
      for(int kk=0;kk<24;kk++){
	tcsiz[kk]->Fill(err*(1.*dblock.ev->event_fftime),dblock.frag_size[kk]);
	if(bb>=0){frsiz->Fill(kk,dblock.frag_size[kk]);
	if(dblock.frag_size[kk]) frsum+=dblock.frag_size[kk];}
      }
      if(bb>=0)frsiz->Fill(24,frsum);
      for(int kk=0;kk<192;kk++){
	if(bb>=0)tdsiz->Fill(kk*1.,dblock.tdr_size[kk]);
	mtcsiz[kk]->Fill(err*(1.*dblock.ev->event_fftime),(1.*dblock.tdr_size[kk]));
      }
    }
    if(bb>=0){
    
      if(dblock.ev->TofFlag1>=0&&dblock.ev->TofFlag1<=8){
	      //if(dblock.ev->TofFlag1==0){
	
      
	// TrClusters 
	if(reco>0){
	  int retr=TrRecon::gethead()->BuildTrClusters();
	  if(retr>=0){
	    int retr2=TrRecon::gethead()->BuildTrRecHits();
	    VCon* cont=TrRecon::TRCon->GetCont("AMSTrRawCluster");
	    int nhits= cont->getnelem();
	    if(cont) delete cont;
 	    if(retr2>=0&& nhits<110){
 	      int retr3=TrRecon::gethead()->BuildTrTracks();
 	    }
	  }
	}
    
    
	
	errcount=0;
	//  trec->SetEvent(dblock.ev);
      
	//       // Reconstruct only TrClusters (AO analysis)
	//       if (reco == 1) {
	// 	//	  trec->BuildTrClusters();
	// 	if(histo>0&&dblock.JMDC.TofFlag1>=0&&dblock.JMDC.TofFlag1<=8){
	// 	  LFillAll(dblock.ev,Histos,0);
	// 	  int frsum=0;
	// 	  for(int kk=0;kk<24;kk++){frsiz->Fill(kk,dblock.frag_size[kk]);if(dblock.frag_size[kk]) frsum+=dblock.frag_size[kk];}
	// 	  frsiz->Fill(24,frsum);
	// 	}
	if(tree>0){
	  t4->Fill();
	}
	nfill++;
      }else count3++;
  
    }
    else{
      if(verbose>0)PRINTF("============================================================== Event with errors %d %d %f\n\n\n",
			  count+1,dblock.ev->event_size,dblock.ev->event_fftime);
      count4++;
      if(histo>0)thsiz2->Fill(dblock.ev->event_fftime,dblock.block_size2);
      if (verbose == 1) POutF::pp->Print();
      errcount++;
      //	if(errcount>=10) {printf("Too many consecutive errors!\n I give up!!\n"); break;}
    }
    if (verbose > 1) POutF::pp->Print();
  }

  
  fclose(filein);
  

  printf("Decoded   %6d events  First event Number  %d  Last event number %d\n",count-skip,evfirst,evlast);
  printf("Filled    %6d events\n",nfill);
  printf("Discarded %6d events\n",count3);
  printf("TOF4/4:  %6d  TOF3/4: %6d  TOF2/4UD:  %6d  TOF2/4:  %6d \n",tof4,tof3,tof21,tof20);

  if(testerr){
      printf("With Errors  %6d events\n",count4);
    
  }
  if(tree){
    t4->Write("",TObject::kOverwrite);
    f->Write();
    f->Close();
  }
  if(histo){
    gg->Write();
    gg->Close();
  }
  
  return 0;
  
}



//---------- SH add ----------
// For the memory leak check

#include <fstream>
#include <string>

float memchk(void)
{
  char path[256];
  sprintf(path, "/proc/%d/status", getpid());
  std::ifstream fin(path);
  if (!fin) return 0;

  char buff[256];
  while (!fin.eof()) {
    fin >> buff;
    if (buff == std::string("VmSize:")) {
      float vmsize;
      fin >> vmsize;
      return vmsize;
    }
  }
  return 0;
}



void  LBookHistos(TrDHF* bb,int flag){

  bb->Define("Occupancy","Occupancy",1024,0,1024);
  bb->Define("RawWidth","RawWidth",100,0,100,1);
  bb->Define("WidthRvsWitdth","WidthRvsWitdth",100,0,100,100,0,100,1);
  bb->Define("Width","Width",100,0,100,1);
  bb->Define("Signal","Signal; ADC counts; events",800,-200,200,1);
  bb->DefineTracker("TofPatt","TofPatt",30,-15,15,0);
  bb->Define("LowTh","LowTh",40,-10,10,1);
  bb->Define("SeedSNR","SeedSNR",160,-20,20,1);
  bb->Define("SeedSN","SeedSN",160,-20,20,1);
  return;
}


void  LFillAll(Event* ev,TrDHF* bb,int flag){
  float th=5;
  bb->FillTracker("TofPatt",ev->TofFlag1);
  //printf("Filling\n");
  for (int ii=0;ii<ev->GetnTrRawClusters();ii++){
    TrRawClusterR* clu=ev->GetTrRawCluster(ii);
    //     if(clu->GetSeedIndexRaw(th)>0)  
    //      bb->Fill(1,clu,"SeedSNR",clu->GetSN(clu->GetSeedIndexRaw(3.5)));
    //    else
    bb->Fill(1,clu,"SeedSNR",-1.);
    bb->Fill(1,clu,"SeedSN",clu->GetSeedSN(3.));
    bb->Fill(1,clu,"RawWidth",clu->GetNelem());	
    if(clu->GetNelem()>3&&clu->GetStatus(1)==0)
      bb->Fill(1,clu,"LowTh",clu->GetSN(1)); 
    bb->Fill(0,clu,"Occupancy",clu->GetSeedAddress(3.5)*1.);
    //      Fill(0,clu,"OccupancyCOG",clu->GetCofG()*1.);
    bb->Fill(1,clu,"WidthRvsWitdth",clu->GetLength(th,1),clu->GetLength(th,-1000)*1.);
    bb->Fill(1,clu,"Width",clu->GetLength(th,1)*1.);
    bb->Fill(1,clu,"Signal",clu->GetTotSignal2(th));
    //      //Fill(1,clu,"SN",clu->GetTotSN()*1.);
  }
  return;
}


void GetTheRightCal(time_t run){

  TrCalDB* cc=new TrCalDB();

  cc->init();
  cc->CreateLinear();

  time_t bb=0;
  tm begin;
  tm end;
  tm* mtim=localtime_r(&bb,&begin);
  cout <<  asctime(&begin)<<endl;
  
  begin.tm_isdst=0;
  begin.tm_sec=0;
  begin.tm_min=0;
  begin.tm_hour=0;
  begin.tm_mday=0;
  begin.tm_mon=0;
  begin.tm_year=0;

  end.tm_isdst=0;
  end.tm_sec=0;
  end.tm_min=0;
  end.tm_hour=0;
  end.tm_mday=0;
  end.tm_mon=0;
  end.tm_year=0;



  AMSTimeID* tt= new 
    AMSTimeID(AMSID("TrackerCals",1),begin,end,
	      TrCalDB::GetLinearSize()
	      ,TrCalDB::linear,AMSTimeID::Standalone,1, SLin2CalDB);


  tm now;
  tm* ptim=localtime_r(&run,&now);
  printf("TEST:   %s \n",asctime(&now));
  tt->validate(run);
  time_t in,bg,en;
  tt-> gettime(in,  bg,  en);  
  tm pout;
  printf("BEGIN:   %s \n",asctime(localtime_r(&bg,&pout)));
  printf("END:     %s \n",asctime(localtime_r(&en,&pout)));
  printf("INSERT:  %s \n",asctime(localtime_r(&in,&pout)));

  return;
}
