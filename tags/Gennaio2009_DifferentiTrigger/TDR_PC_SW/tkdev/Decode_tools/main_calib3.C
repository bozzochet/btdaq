
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
#include "POutF.h"

typedef  unsigned short int ushort;

int verbose=0;
int goodrun=0;
time_t endtime=0;
char block_dir[120];
char outdir[120];

int  SaveCal2DB(TrCalDB * cc,time_t endtime);


void _usage(char * nn);
void _usage(char *name) {
  printf("Usage:\t %s [options] -r runnum  \n",name);
    printf("Options:\n");
    printf("\t-h -? --help              \tprint this message\n");
    printf("\t-B    --blockdir <path>   \tthe dir where to search for blocks. Default is %s \n",block_dir);
    printf("\t-O    --outputdir <path>  \tthe dir where to put the output files. Default is %s \n",outdir);
    printf("\t-D    --dbase <endtime>   \tWrite the cal to the dbase putting <endtime> as end of validity\n");


    exit(0);
}


int _proc_args(int argc, char *argv[]);

//----------------------------------------------------------
int _proc_args(int argc, char *argv[]) {
    int c;
    int option_index = 0;
    static struct option long_options[] = {
        {"help",     0, 0, 'h'},
        {"Run",      1, 0, 'r'},
        {"blockdir", 1, 0, 'B'},
        {"outputdir", 1, 0,'O'},
        {"dbase"    , 1, 0,'D'},
        {0, 0, 0, 0}
    };
    char logfile[100] = "";
    char *ptr;

    /* Turn off error check for getopt */
    opterr = 0;

    /* Loop on options */
    while (1) {
      c = getopt_long (argc, argv, "hr:D:O:B:?", long_options, &option_index);
      if (c == -1) break;
      
      switch (c) {

      case 'r':             /* logdir */
	goodrun=atoi(optarg);
	break;
      case 'B':
	sprintf(block_dir,"%s",optarg);
	break;
      case 'O':
	sprintf(outdir,"%s",optarg);
	break;
      case 'D':
	endtime=atoi(optarg);
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
  struct stat buf;
  
  char fname[120]; 
  char fullfname[120]; 
  char fullfname2[120]; 
  char foutname[120]; 
  int nfiles=0;
  int istat;
  new POutF();
  sprintf(outdir,"./runs/");
  sprintf(block_dir,"./blocks/0011");

//   /* Process command line input arguments */
  istat = _proc_args(argc, argv);
  
//   /* Error from command line */
  if ( istat < 0 ) {
    printf("Error: from the command line\n");
    exit(1);
  }


  
  sprintf(foutname,"%s/CalDB_%d.root",outdir,goodrun);
   
  //sprintf(fullfname,"%s/%dC.bin",block_dir,goodrun);
  sprintf(fullfname,"%s/%d",block_dir,goodrun);
  

  // Create and Init the Tracker Database
  TkDBc::CreateTkDBc();
  TkDBc::Head->init();
  //int readcalibration(char *fname)
 // Open the output root file
  TFile f(foutname,"RECREATE");

  //Create a Calibration database
  DeBlock dblock;
  dblock.caldb= new TrCalDB();
  dblock.caldb->init();
  dblock.caldb->run=goodrun;
  

  dblock.ev= new Event();


  ushort ss;
  int aa;
  int count=-1;
  int count2=0;
  FILE* filein= fopen(fullfname,"r");
  if(!filein){
    printf("FATAL.  Problems opening  %s\n",fullfname);exit(2);
  }
  
  dblock.SetInputFile(filein);
  while(!feof(dblock.rawfile)){
    // while(count<10){
    POutF::pp->Clear();
    count++;
    PRINTF("-----------------------------------------------------\n");
    PRINTF("---------------- BLOCK #%d --------------------------\n ",count);
    PRINTF("-----------------------------------------------------\n");
    dblock.ev->Clear();
    aa=dblock.ReadOneBlock();
    if(aa<0) break;
    dblock.DecodeBlockHeader();
    dblock.bh1.Print();    
    if(dblock.bh1.type!=6) continue; //the type calibration
    dblock.DecodeJMDCSubBlock();
    dblock.JMDC.Print();
    
    dblock.ev->runnum      = dblock.JMDC.runnum;
    dblock.ev->runtag      = dblock.JMDC.runtag;
    dblock.ev->eventnum    = dblock.JMDC.eventnum;
    dblock.ev->event_time  = dblock.JMDC.event_time;
    dblock.ev->event_ftime = dblock.JMDC.event_ftime;
    
    SubBlock aa;
    int offset=dblock.JMDC.last_word;
    PRINTF("===============================================\n");
    dblock.DecodeSubBlockHeader(&aa,offset);
    aa.Print();

    if(aa.node <282 || aa.node>473) continue ; //the TDR range
    count2++;
    int joffset=aa.FirstDword-1;
    int jsize=aa.LastDword-aa.FirstDword+2;
    dblock.DecodeOneCal(&aa,joffset,jsize);
    
    PRINTF("\n");
    POutF::pp->Print();
  }
  fclose(filein);
  

  //  if(endtime)SaveCal2DB(dblock.caldb,endtime);
  printf("Decoded %d Calibration objects\n",count2);
  dblock.caldb->Write("",TObject::kOverwrite);
  f.Write();
  f.Close();
  
  return 0;

}



