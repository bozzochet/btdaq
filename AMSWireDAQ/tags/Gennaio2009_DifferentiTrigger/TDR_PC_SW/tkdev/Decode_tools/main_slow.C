#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
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

typedef  unsigned short int ushort;
int goodrun=0;
char block_dir[320];
char outdir[320];
char calfname[120];
char caldir[120];
char dbcname[120] = "";
int maxev=999999999;
int verbose=0;  
int skip=0;
float memchk(void);



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
  printf("\t-V    --verbose <level>  \tverbose level (0: quiet, 1: report errors 2: full), default 0 \n");
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
    {"maxev",      1, 0, 'M'},
    {"skip",      1, 0, 'S'},
    {"verbose",      1, 0, 'V'},
    {"Run",      1, 0, 'r'},
    {"blockdir", 1, 0, 'B'},
    {"outputdir", 1, 0,'O'},
    //    {"nocnoise", 0, 0,'N'},
    {0, 0, 0, 0}
  };

  /* Turn off error check for getopt */
  opterr = 0;

  /* Loop on options */
  while (1) {
    c = getopt_long (argc, argv, "hH:r:O:S:M:B:V:?", long_options, &option_index);
    if (c == -1) break;

    switch (c) {

      case 'r':             /* logdir */
	goodrun=atoi(optarg);
	break;
      case 'V':             /* logdir */
	verbose=atoi(optarg);
	break;
      case 'M':             /* logdir */
	maxev=atoi(optarg);
	break;
      case 'S':             /* logdir */
	skip=atoi(optarg);
	break;
      case 'B':
	sprintf(block_dir,"%s",optarg);
	break;
      case 'O':
	sprintf(outdir,"%s",optarg);
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

int main(int argc, char ** argv){ 


  char calibname[120]; 
  char fullfname[120]; 
  char foutname[120];
  int istat;

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


  new POutF();

  DeBlock dblock;
  dblock.ev= new Event();



  sprintf(fullfname,"%s/%d",block_dir,goodrun);
  int aa;
  int count=-1;
  int count3=0;
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

  dblock.SetInputFile(filein);
  while(!feof(dblock.rawfile)){
    //while(count<10){
    count++;
    POutF::pp->Clear();
    dblock.Clear();
    if(count>maxev) break;

    if (verbose > 0) {
      PRINTF("-----------------------------------------------------\n");
      PRINTF("---------------- BLOCK #%d --------------------------\n ",count);
      PRINTF("-----------------------------------------------------\n");
    }
    dblock.ev->Clear();

    aa=dblock.ReadOneBlock();
    if(aa<0) break;
    dblock.DecodeBlockHeader();
    if (verbose > 0) dblock.bh1.Print();    
    //    if(dblock.bh1.type!=5) {POutF::pp->Print(); continue;} //the type data
    // if(dblock.bh1.node!=15) {POutF::pp->Print();continue;} //the right JMDC
    dblock.ScanBlocksSlow();
    if(dblock.bh1.node==0x82)POutF::pp->Print();
    nfill++;

  } 	

  

  fclose(filein);


  printf("Decoded   %6d events  First event Number  %d  Last event number %d\n",count-skip,evfirst,evlast);
  printf("Filled    %6d events\n",nfill);
  printf("Discarded %6d events\n",count3);

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



