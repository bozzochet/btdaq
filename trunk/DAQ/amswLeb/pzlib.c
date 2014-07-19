// file paolo-amsw.c
//
//
// A.Lebedev, Mar-2009...
//
// A.Basili May-2009...
// P.Zuccon May-2009...
#include "mylib.h"
#include "amswlib.h"


#include "pzlib.h"

int send_AMSW_CMD(int port, int cmdlen, unsigned short* cmd, unsigned short* Event, int* usize) {
  NODE_ADR = 0x10;     //global variable JMDC Node num
  //int16 JINJ_add=0x001;not mode needed cause it was passed trough the "port"
  //  P=3;
  int16 JINJ_add[4]={0x001, 0x102, 0x204, 0x0308};
  int kk;
  int16 RQ_dat[MAXPARS]; //ARRAY OF PARAMETERS
  memset(RQ_dat,0,MAXPARS*sizeof(RQ_dat[0]));

  if(cmdlen>MAXPARS) return -1;
  RQ_dat[0]=JINJ_add[port];
  for(kk=0;kk<cmdlen;kk++) RQ_dat[kk+1]=cmd[kk];

  //----for debug purpose------------
  //  for(kk=0;kk<cmdlen+1;kk++) printf("%04X ", RQ_dat[kk]);
  //  printf("\n");
  //---------------------------------
  
  setup_command_path_from_file("slowconf.conf");

  int32 RQ_typ = 0x1f0381; // Request command or data type
  int32 RQ_cnt = cmdlen+1;   //Number of request parameters


  int32 RP_siz=ANSW_LIM;   //Max acceptable answer lenght
  int32 RP_cnt=0;          //Actual Answer lenght  
  int16 RP_dat[RP_siz];    //Answer 
  int16 RP_err=0;          //Error code

  to_AMSW(READ,  RQ_typ, RQ_cnt, RQ_dat,  RP_siz, &RP_cnt, RP_dat, &RP_err);
  *usize=-1;
  if (RP_cnt < RP_siz){
    for( kk=0;kk<RP_cnt;kk++) {	
      Event[kk]=RP_dat[kk];
    }
    *usize=RP_cnt;
  }
  else  
    printf("Warning reply truncated beacuse too large: %d limit is %d \n",RP_cnt,RP_siz);
 
  if (RP_err) 
    return RP_err;
  else
    return 0; 
}

void Print_Level(int level) {
  P = level;  //DEBUG global variable set >0 to have debug info
  return;
}
