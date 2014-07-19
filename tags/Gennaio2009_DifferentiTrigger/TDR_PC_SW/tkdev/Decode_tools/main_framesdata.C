#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#include <arpa/inet.h>
#include "Deframe.h"
#include <unistd.h>

#include <getopt.h>
void _usage(char * nn);
int _proc_args(int argc, char *argv[]);

char source_dir[255];
char dirout[255];
int firstfile=0;
int lastfile=0;


int main(int argc, char ** argv){
  char fullfname[255];
  int istat;
  int nfiles=0;
  int pri=1;

  sprintf(source_dir,"./frames/0011/");
  sprintf(dirout,"./blocks/0011/");
  
  
  //   /* Process command line input arguments */
  istat = _proc_args(argc, argv);
  
  //   /* Error from command line */
  if ( istat < 0 ) {
    printf("Error: from the command line\n");
    exit(1);
  }



 
  
  if(lastfile) nfiles=lastfile-firstfile+1;
  else  nfiles=1;
    
  if(nfiles<0) {printf("FATAL given file range  is wrong!!! \n"); exit(2);}
  for (int ii=0;ii<nfiles;ii++){

    sprintf(fullfname,"%s/%03d",source_dir,firstfile+ii);    
    FILE* filein=fopen(fullfname,"r");
    if(!filein){printf("Fatal: cannot open the input file %s\n",fullfname); exit(2);}
    else printf("File %s has been found!\n",fullfname);
    fclose(filein);
  }

  Deframe df(0,dirout);
  df.goodnode=16; // JMDC0
  df.goodrun=0;
  int goodAPID=976;

  int aa;
  int count=-1;
  int block_done=1;
  int first=1;
  FILE* filein;
  for (int kk=0;kk<nfiles;kk++){
    sprintf(fullfname,"%s/%03d",source_dir,firstfile+kk);    
    filein=fopen(fullfname,"r");
    if(!filein)
      {printf("Fatal: cannot open the input file %s\n",fullfname); exit(2);}
    df.SetInputFile(filein);
     
    for(;;){
      count++;
      
      //  Decode Frame  
      printf("-----------------------------------------------------------------------------------------------\n");
      //printf("---------        FRAME #%d   ------------------------\n",count);
      //     printf("-----------------------------------------------------\n");
      printf("FRAME #%d ",count);
      df.HeaderWords=0;
      df.CRCwords=0;
      // Read the primary header and copy the frame to the memory array frame[]
      aa=df.ReadOneFrame();
      if(aa<0) break;
      df.fh1.Print();
    
      //Read the CCSD Secondary Header (if it is Present)
      if(df.fh1.SHF){
	df.DecodeFrameSecondaryHeader();
	df.fh2.Print();
      }
      // Read the CCSD AMS Header
      df.DecodeAMSHeader();  
      df.amsh.Print();
      if(df.fh1.APID!=goodAPID) continue;
      if(df.amsh.block_offset==0xFFF){  // Fill Frame
	if(pri)printf("Fill frame Ignore it \n");
      }
      else if(df.amsh.block_offset==0xFFE){  // Continution Frame
	if(first)  continue;
	
	if(pri)	printf(" Continuation block %d ... \n",df.block_size);
	  aa=df.WriteTheFrame(df.HeaderWords+1);
	if(aa<0) break;
	if(!block_done&&pri)printf(" Continuation block ends %d ... \n",df.block_size);
      }
    
      else if(df.amsh.block_offset==0){  // new block
	aa=df.BlockScan(df.HeaderWords+1);
	first=0;
	if(aa<0) return aa;
      }
      else{ //we must probably complete the previous block and start a new one at the offset
	if(first){
	  char fullout2fname[140];
	  sprintf(fullout2fname,"%s/%03d_orphan",dirout,firstfile);
	  //	  FILE* outfile2=fopen(fullout2fname,"w");
	  FILE* outfile2=0;
	  int written=0;
	  if(outfile2){
	    int start=df.HeaderWords+1; int end=df.HeaderWords+1+df.amsh.block_offset;
	    for (int yy=start;yy<end;yy++){
	      if(df.writeOneWord(df.frame[yy],outfile2)) 
		written++;
	    }
	    if(pri)printf(" Written  %d words to the orphan file %s ... \n",written,fullout2fname);
	  }
	  //else{printf("====================>>>>> Warning cannot open %s \n",fullout2fname);}
	}else{
	  if(df.block_size&&pri) printf("End Part block %d ... \n",df.block_size);

	  aa=df.WriteTheFrame(df.HeaderWords+1,df.HeaderWords+1+df.amsh.block_offset);
	  if(aa<0) break;
	}
	
	first=0;
	if(df.HeaderWords+df.amsh.block_offset+1>=df.fh1.packet_len-1) continue;
	aa=df.BlockScan(df.HeaderWords+df.amsh.block_offset+1);
	if(aa<0) break;
      }

      if(aa<0) break;      
      
    }

    if(filein) fclose(filein);

  }

  return 0;
    
  
}



void _usage(char *name) {
  printf("Usage:\t %s [options]  -F first_blockfile  [-L last_blockfile] \n",name);
    printf("Options:\n");
    printf("\t-h -? --help              \tprint this message\n");
    printf("\t-B    --framesdir <path>  \tthe dir where to search for frames. Default is %s \n",source_dir);
    printf("\t-O    --outputdir <path>  \tthe dir where to put the output files. Default is %s \n",dirout);
    printf("\t-L    --last <filenumber> \tused to define a range of files to be decoded. Default last_file==first_file\n");
    exit(0);
}



//----------------------------------------------------------
int _proc_args(int argc, char *argv[]) {
    int c;
    int option_index = 0;
    static struct option long_options[] = {
        {"help",     0, 0, 'h'},
        {"blockdir", 1, 0, 'B'},
        {"outputdir", 1, 0,'O'},
        {"first",    1, 0, 'F'},
        {"last",     1, 0, 'L'},
        {0, 0, 0, 0}
    };

    /* Turn off error check for getopt */
    opterr = 0;

    /* Loop on options */
    while (1) {
      c = getopt_long (argc, argv, "hO:B:F:L:?", long_options, &option_index);
      if (c == -1) break;
      
      switch (c) {

      case 'B':
	sprintf(source_dir,"%s",optarg);
	break;
      case 'O':
	sprintf(dirout,"%s",optarg);
	break;
      case 'F':
	firstfile=atoi(optarg);
	break;
      case 'L':
	lastfile=atoi(optarg);
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
