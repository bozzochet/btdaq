// class created by Giacomo Galilei jan-feb 2005
//
// modified by phazz in may-june 2005 to use new rom monitor,
// and to include it into Eduardo Cortina's TDR program.
//
// sept 2005 phazz, trying to include Florian Hauler's work into this class, to add PCI
//
// 7.10.2005 phazz: modifying the class to be able to receive more than one fragment
//                  i.e. more than 8 KB of data in a single event.
// 9.10.2005 phazz: starting a big cleanup of the class
//
// 29.9.2006 phazz: pci can send more than one frag of data
// 1.10.2006 phazz: epp can send more than one frag of data
// 2.10.2006 phazz: write flash works
// 9.08.2007 pzuccon: general cleanup of include files and remove of some oddities
//22/08.2007 pzuccon: Import from TestPCI: all the printout for the interactive use
//                                         the method to call a command from its hex code
//                                         update the output redirection method via a pointer to a function
//

#include<iostream>

#include <errno.h>
#include <asm/bitops.h>
#include <sys/io.h> 
#include <sys/time.h>
#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include <sstream>

//Definition of AMSWcom class
#include "amswcom.h"

//C/C++ API for the amsw commands
#include "amsw_lib.h"


using namespace std;




#ifdef WITH_COL
# define COLCHAR           1
# define COL               "\001"
# define COL_NORMAL        COL "00"
# define COL_EVIDENT   COL "01"
# define COL_STRONG        COL "02"
#else
# define COLCHAR         1
# define COL             ""
# define COL_NORMAL      ""
# define COL_EVIDENT     ""
# define COL_STRONG          ""
#endif



bool AMSWcom :: eppUp = false;



// * Constructor
AMSWcom::AMSWcom(int portnum, int hardware, int card) {
  hwtype=hardware;
  port=portnum;
  pcicard=card;
  sType=kBATCH;
  
  EventSize=0;
  for( int i = 0; i < EVENTSIZE; i++ ) {
    Event[i] = 0;
    if (i<1024) {
      sig[i]=0;
      sigr[i]=0;
      ped[i]=0;
      sta[i]=0;
    }
  }

  Output=AMSWcom::OutputStd;
  TimeOut=5000; // time in ms
  DEBUG=0;
  memset(FlashSummary,0,sizeof(FlashSummary));
  memset(CmdArray,0,sizeof(CmdArray));
  //printf("new amsw: type=%d, port=%d, card=%d\n",hwtype, port, card);

  //  for (int tdr=0; tdr<24; tdr++) { // default settings for 0x3123 program
    CalParMem.Ped=0x1300;
    CalParMem.MemPed=kDSP_PM;
    
    CalParMem.SigH=0x1b04;
    CalParMem.MemSigH=kDSP_PM;
    
    CalParMem.SigL=0x1b04;
    CalParMem.MemSigL=kDSP_PM;
    
    CalParMem.Stat=0x1702;
    CalParMem.MemStat=kDSP_PM;
    
    CalParMem.Sigr=0x1702;
    CalParMem.MemSigr=kDSP_DM;
    
    CalParMem.SigHFactor=1.0;
    CalParMem.SigLFactor=1.0;
    CalParMem.SigrFactor=1.0;
    //  }
    PrepareFCSTable();
}




// * Destructor
AMSWcom::~AMSWcom( ) {
  return;
}


void AMSWcom::SetDEBUG(int mode) {
  DEBUG=(mode)?1:0;
}

void AMSWcom::SetHW(int hardware) {
  hwtype=hardware;
}


void AMSWcom::SetPort(int portnum) {
  port=portnum;
}


bool AMSWcom::Init() {

  bool ret=SUCCESS;

  switch(hwtype) {
    
  case kAMSW_EPP:
    if (DEBUG) printf("Init... via EPP\n");
    ret=InitEpp();
    if (DEBUG) printf("ret=%d\n",ret);
    break;
    
  case kAMSW_PCI:
    if (DEBUG) printf("Init... via PCI\n");
    ret=(amsw_open(pcicard)<0)?ERROR:SUCCESS;
    break;
  }

  return ret;

}



// * Static methods to bring up and down the epp connection
bool AMSWcom :: InitEpp ( )
{

//      if( eppUp )
//              return SUCCESS;
        
//      if( ioperm( EPPBASE, 8, true ) )
//              return ERROR;

//      unsigned char stat_byte = 0x0, cont_byte = 0x0;

//      // *  reset EPP
//      outb(cont_byte, EPPCONTROL);
//      usleep(1000);
//      set_bit(C2, (unsigned long *)&cont_byte);
//      outb(cont_byte, EPPCONTROL);

//      // *  clear timeout bit
//      stat_byte = inb(EPPSTATUS);
//      if (test_bit(S0, (unsigned long *)&stat_byte))
//              outb(stat_byte, EPPSTATUS);

//      eppUp = true;

//      return SUCCESS;


  if(eppUp)
    return SUCCESS;
  
  if (iopl(3)) {                    // get I/O permission
    //perror("EPP: iopl open problems ");
    return ERROR;
  }


  unsigned char stat_byte = 0x0, cont_byte = 0x0;
  
  // *  reset EPP
  outb(cont_byte, EPPCONTROL);
  usleep(1000);
  //set_bit(C2, (unsigned long *)&cont_byte);
  //outb(cont_byte, EPPCONTROL);
  outb(0x04, EPPCONTROL);
  outb(0x84, ECRCONTROL);     // some magics, helped Bilge, needs iopl(3)
  
  // *  clear timeout bit
  stat_byte = inb(EPPSTATUS);
  //if (test_bit(S0, (unsigned long *)&stat_byte))
  if ((1<<S0) & stat_byte) outb(stat_byte, EPPSTATUS);
  
  eppUp = true;
  
  return SUCCESS;
}



bool AMSWcom::Shut() {

  bool ret=SUCCESS;

  switch(hwtype) {

  case kAMSW_EPP:
    ret=ShutEpp();
    break;

  case kAMSW_PCI:
    // to be defined
    amsw_close(pcicard); // use with care !
    break;
  }

  return ret;
}


bool AMSWcom :: ShutEpp ( )
{


	unsigned char cont_byte = 0x0;

	if( eppUp == false )
		return SUCCESS;

	// *  assert reset bit
	outb(cont_byte, EPPCONTROL);

	if( ioperm(EPPBASE, 8, false ))
		return ERROR;

	eppUp = false;
	
	return SUCCESS;
}



void AMSWcom::SetCalParMem(CalibMemory *calmem) {
  //printf("lowfactor=%lf  highfactor=%lf\n", calmem->SigLFactor,calmem->SigHFactor);
  memcpy(&CalParMem,calmem,sizeof(CalParMem));
  //printf("lowfactor=%lf  highfactor=%lf\n", CalParMem.SigLFactor,CalParMem.SigHFactor);
}



void AMSWcom::Command(int args, unsigned short *params, int mode24) {
  switch (hwtype) {
  case kAMSW_EPP:
    CommandEPP(args,params,mode24);
    break;
  case kAMSW_PCI:
    CommandPCI(args,params,mode24);
    break;
  }
}

void AMSWcom::Command2(unsigned int addrl, unsigned char cmd, int args, ushort* params) {

  unsigned short addr=addrl & 0xffff;
  unsigned short addr2=addrl>>16;

  int cnt=0;
  int mode24=0;

  if (args>(CMDSIZE-1)) { printf("too many args\n"); return; }

  if (addr!=0xffff) {
    CmdArray[cnt++]=addr; // addr can be 0xYY3f or 0x4000 or ... 
    if (addr2) CmdArray[cnt++]=addr2; // secondary address, useful if you use e.g. a Jinj.
  }

  CmdArray[cnt++]=0x2e00+cmd;

  for( int i = 0; i < args; i++ ) CmdArray[cnt++] = params[i];

  if (cmd==cAMSW_READPM) {
    cnt--;
    mode24=CmdArray[cnt];
  }


  Command(cnt, CmdArray,mode24); // if readpm, mode may be set to 24 bits
}

void AMSWcom::Command(unsigned int addrl, unsigned char cmd, int args, ... ) {

  unsigned short addr=addrl & 0xffff;
  unsigned short addr2=addrl>>16;

  int cnt=0;
  int mode24=0;

  if (args>(CMDSIZE-1)) { printf("too many args\n"); return; }

  if (addr!=0xffff) {
    CmdArray[cnt++]=addr; // addr can be 0xYY3f or 0x4000 or ... 
    if (addr2) CmdArray[cnt++]=addr2; // secondary address, useful if you use e.g. a Jinj.
  }

  CmdArray[cnt++]=0x2e00+cmd;

  va_list ap;
  va_start( ap, args );
  for( int i = 0; i < args; i++ ) CmdArray[cnt++] = (unsigned short)va_arg( ap, int );
  va_end( ap );


  if (cmd==cAMSW_READPM) {
    cnt--;
    mode24=CmdArray[cnt];
  }


  Command(cnt, CmdArray,mode24); // if readpm, mode may be set to 24 bits
}


void AMSWcom::CommandPCI(int args, unsigned short *params, int mode24) {
  //  unsigned char cmdch[2*];
  unsigned short val=0;



  int j=0; // command array counter

  for( int i = 0; i < args; i++ ) {
    val = params[i];
    //printf("0x%04x\n",params[i]);
    cmdch[j++]=val >> 8;
    cmdch[j++]=val & 0xff;
  }


  //for (int i=0; i<j; i+=2) printf("%02x%02x ", cmdch[i],cmdch[i+1]);
  //printf("\n");

  RX_DONE=SendCommandPCI(port, cmdch, j, Event, &EventSize, mode24);

  if (EventSize>0) { BuildStat=Event[EventSize-1]; CRC=Event[EventSize]; }
  else { BuildStat=0; CRC=0; }
 
}


void AMSWcom::CommandPCI (unsigned int addrl, unsigned char cmd, int args, ... ) {

  unsigned short addr=addrl & 0xffff;
  unsigned short addr2=addrl>>16;



  int cnt=0;

  if (args>(CMDSIZE-1)) { printf("too many args\n"); return; }

  if (addr!=0xffff)  {
    CmdArray[cnt++]=addr; // addr can be 0xYY3f or 0x4000 or ...
    if (addr2) CmdArray[cnt++]=addr2; // secondary address, useful if you use e.g. a Jinj.    
  }
 
  CmdArray[cnt++]=0x2e00+cmd;

  va_list ap;
  va_start( ap, args );
  for( int i = 0; i < args; i++ ) CmdArray[cnt++] = (unsigned short)va_arg( ap, int );
  va_end( ap );


  CommandPCI(cnt, CmdArray,(cmd==cAMSW_READPM)); // if readpm, mode is set to 24 bits
}



/************************************************************************
       Function:
               SendCommand - Send Command including parameters

      RETURNS:
              0  success
            - 1  link is not ready, timeout  
            - 2  TX failure
            - 3  link is not ready for receiving, timeout
            - 4  RX failure
            - 5  command error         
            - 6  CRC error

*************************************************************************/

int AMSWcom::SendCommandPCI(int link, const u_char command[], int csize, u_short wordarray[], int *datasize, int mode24){
  static int timeout  = 200000000;  

  u_char data[8192*3]; // extending the data length to 24 KB
  int rx_bc; 
  int tcounter;   //timeout counter
  int ret;     
  int fcounter=0; // fragment counter
  int fragsize=0; // fragment size

  *datasize=0;

  /* Start TX */
  
  //if (verbose == 1) logmsg("AMSW_TEST","Preparing TX: \n");
  
  tcounter = 0;
  struct timeval tv1,tv2;
  gettimeofday(&tv1,0);
  double elapsed=0;


  while ( amsw_tx_ready(link) <= 0 ) {
    if (tcounter>=timeout) return -1; 
    printf("%s: Waiting for free link %i\n",__FUNCTION__,amsw_tx_ready(link));  
    tcounter++;
  }
  
  /* Send data out */	  

  // we have to take into account the possibility that we transmit more than one fragment of data...

  unsigned short bc=2; // bloc control: 2=begin of block, 
  unsigned short position=0;
  unsigned short limit=2*0xfff;
  while (csize>limit) { // more than one fragment
    ret = amsw_tx_packet(link, &command[position], limit, bc);
    //printf("csize=%x bc=%d  ret=%d\n",csize,bc,ret);
    if ( ret < 0 ) return -2; //TX failed
    csize-=limit;
    bc=0;
    position+=limit;

    tcounter = 0;
    while ((fragsize=amsw_rx_ready(link)) < 0) {
      //if (verbose == 1) logmsg("AMSW_TEST","Waiting for free link %i\n",*datasize); 
      //     if (tcounter>=timeout) return -3; 
      tcounter++;
      if (tcounter%100) {
	gettimeofday(&tv2,0);
	elapsed=(tv2.tv_sec-tv1.tv_sec)*1000+ (tv2.tv_usec-tv1.tv_usec)/1000;
	if (elapsed>TimeOut) return -3; // TimeOut in ms
      }
    }
    //printf("tcounter=%d, fragsize=%d\n",tcounter, fragsize);

    rx_bc = 0;
    ret = amsw_rx_packet(link, &data[*datasize], fragsize, &rx_bc);

    *datasize=0;

    //printf("ret=%d, rx_bc=%d\n",ret,rx_bc);
  }
  
  bc|=1; //we add end of block
  
  ret = amsw_tx_packet(link, &command[position], csize, bc);
  //printf("csize=%x bc=%d  ret=%d\n",csize,bc,ret);

  if ( ret < 0 ) return -2; //TX failed
  
  /* Receive reply */
  tcounter = 0;

  elapsed=0;

  do {
    if (tcounter>0) ret=amsw_tx_packet(link, command, 0, 0); // next fragment, please !
    while ((fragsize=amsw_rx_ready(link)) < 0) {
      //if (verbose == 1) logmsg("AMSW_TEST","Waiting for free link %i\n",*datasize); 
      //     if (tcounter>=timeout) return -3; 
      tcounter++;
      if (tcounter%100) {
	gettimeofday(&tv2,0);
	elapsed=(tv2.tv_sec-tv1.tv_sec)*1000+ (tv2.tv_usec-tv1.tv_usec)/1000;
	if (elapsed>TimeOut) return -3; // TimeOut in ms
      }
    }

    /* Get RX data */
  
    //if (verbose == 1) logmsg("AMSW_TEST","Datasize %i\n",*datasize);
    //if (verbose == 1) logmsg("AMSW_TEST","Ready to get RX-packet\n");
  
    rx_bc = 0;
    ret = amsw_rx_packet(link, &data[*datasize], fragsize, &rx_bc);
    //if (verbose == 1) logmsg("AMSW_TEST","RET-value of RX  is %i\n", ret);
    //  printf("BC-Bits are: %d\n",rx_bc);
    
    *datasize+=fragsize;
    
    if ( ret < 0 ) return -4;  //RX failed ...

  /* Check data */
  if (rx_bc!=3 && (fragsize==0)) {
    //printf("rx_bc=%d\n",rx_bc);
    return -5; // simplified check, temporary
  }
      
  // now the block control is important, we need to get data until we get end of block...  
  fcounter++;
  } while ((rx_bc & 1)==0); // we are waiting for 'end of block'
  

  if (*datasize > 0) {
    CRC=(u_short(data[(*datasize)-2])<<8)+u_short(data[(*datasize)-1]);
    //printf("PCI: CRC=0x%04x   DoFCS= 0x%04x\n",CRC,DoFCS(data, (*datasize)-2));
    if (CRC!=DoFCS(data, (*datasize)-2)) return -6; // CRC check

    if (mode24) byte24_to_word(data, datasize, wordarray);
    else byte_to_word(data, datasize, wordarray);
  }
  return 0;
}



// /************************************************************************
//        Function:
//                SendCommand24 - Send Command including parameters

//       RETURNS:
//               0  success
//             - 1  link is not ready, timeout  
//             - 2  TX failure
//             - 3  link is not ready for receiving, timeout
//             - 4  RX failure
//             - 5  command error         
//             - 6  CRC error

// *************************************************************************/




int AMSWcom::Convert24() {

  int count=0;
  int totlen=EventSize-1;
  unsigned int val1=0, val2=0;

  for(int i=0;i<totlen-2;i+=3) {
    if (DEBUG) printf("%04x %04x %04x\n",Event[i],Event[i+1],Event[i+2]);
    val1=(Event[i]<<8) | (Event[i+1]>>8);
    val2=(( 0x00FF & Event[i+1])<<16) | Event[i+2];
    if (DEBUG) printf("%04x %04x\n", val1, val2);
    Event[count++]=(unsigned short) (val1>>8);
    Event[count++]=(unsigned short) (val2>>8);
  }
  return count;
}




/************************************************************************
       Function:
               byte_to_word - Converts an array of bytes into an array of words

          RETURNS:
                 none
 ************************************************************************/
void AMSWcom::byte_to_word(u_char bytearray[], int *size, u_short wordarray[]) {
  int i,j;
  j=0;
  
  for (i = 0; i < *size; i=i+2){
    if ((i+1)<*size) wordarray[j] = bytearray[i]*256+bytearray[i+1];
    else wordarray[j] = bytearray[i]*256;
    j++;
  }
  *size = j-1;
  return;
}


/***********************************************************************

Function 24byte_to_16


************************************************************************/


void AMSWcom::byte24_to_word(u_char bytearray[], int *size, u_short wordarray[]) {
  int i,j;
  j=0;
  
  for (i = 0; i < *size; i=i+3){
    if ((i+1)<*size) wordarray[j] = bytearray[i]*256+bytearray[i+1];
    else wordarray[j] = bytearray[i]*256;
    j++;
  }
  *size = j-1;
  return;


}



void AMSWcom :: Build_stat ()
{
  //FOutput ( "Command " COL_EVIDENT "%s" COL_NORMAL ", BuildStat " COL_EVIDENT "0x%04x\n", command, inw(EPPDATA));
  BuildStat=inw(EPPDATA);
  // * extract the FCS too
  inw(EPPDATA);
}


void AMSWcom::SetOutput( OUTFUN aa){

  Output=aa;
}


void AMSWcom :: OutputStd( std::string s )
        {

  std::cout<<s;
  return;
        }

void AMSWcom :: OutputFile( std::string s )
{

  return;
}


void AMSWcom :: FOutput ( char * fmt, ... )
{
	char msg[1024];

	va_list ap;

	va_start( ap, fmt );
	vsnprintf( msg, sizeof(msg), fmt, ap );
	va_end( ap );

	string s;
	for( unsigned int i = 0; i < strlen(msg); i++ )
		s += msg[i];
	
	Output( s );

	return;
}


void AMSWcom :: IOutput ( char * fmt, ... )
{
        char msg[1024];

        va_list ap;

        if(sType==kBATCH) return;
        va_start( ap, fmt );
        vsnprintf( msg, sizeof(msg), fmt, ap );
        va_end( ap );

        string s;
        for( unsigned int i = 0; i < strlen(msg); i++ )
                s += msg[i];
        
        Output( s );

        return;
}



void AMSWcom::Boot(unsigned int addr, unsigned short fname) {
  if (DEBUG) printf("welcome to boot2...\n");

  IOutput("Booting address 0x%08x \n",addr);
  if(fname!=0){
    IOutput(" with program 0x%04x\n", fname); 
    Command(addr, cAMSW_BOOT, 1, fname );
  }
  else
  Command(addr, cAMSW_BOOT, 0);


  PrintRXDONE();

}

void AMSWcom::PrintRXDONE() {
  if (hwtype==kAMSW_EPP)  IOutput("rxdone = %04x\n",GetRX_DONE());
  if (hwtype==kAMSW_PCI) IOutput("rxdone = %d\n",(short) GetRX_DONE());
}


void AMSWcom::SetSSF(unsigned int addr, unsigned short setting) {
  IOutput("Setting SSF mode to %x for address 0x%08x\n",setting,addr);

  Command(addr, cAMSW_SSFWR, 1, setting);
  PrintRXDONE();
}


unsigned short AMSWcom::GetSSF(unsigned int addr) {

  Command(addr, cAMSW_SSFRD, 0);
  unsigned short mode=Event[0];
  IOutput("Address 0x%08x has the SSF in mode %x\n",addr, mode);
  PrintRXDONE();

  return Event[0];
}


void AMSWcom::SlaveTest(unsigned int addr) {
  IOutput("Testing slaves of address 0x%08x\n",addr);
  Command(addr, cAMSW_SLAVTST, 0);
  PrintRXDONE();
}




void AMSWcom::FlashLoad(unsigned int addr, unsigned short fname) {
  IOutput("Address 0x%08x will load program id 0x%04x\n",addr,fname);
  Command(addr, cAMSW_FLASHLD, 1, fname );
  PrintRXDONE();

}


void AMSWcom::FlashRead(unsigned int addr, unsigned short fname,char* nameout) {

  int ret=0;

  if(nameout)FOutput("Saving file 0x%04x of address 0x%08x in %s \n",fname,addr,nameout);
  else FOutput("Saving file 0x%04x of address 0x%08x in file.dat\n",fname,addr);

  Command(addr, cAMSW_FLASHRD, 1, fname );
  PrintRXDONE();
  ret=SaveFlashFile(nameout);
  if(ret)FOutput("ERROR\n"); else FOutput("SUCCESS\n");

}



void AMSWcom::FlashErase(unsigned int addr, unsigned short fname) {
  IOutput("Address 0x%08x will erase program id 0x%04x\n",addr,fname);
  Command(addr, cAMSW_FLASHERASE, 1, fname );
  PrintRXDONE();
}


void AMSWcom::FlashDefault(unsigned int addr, unsigned short fname, unsigned short def) {
  IOutput("Address 0x%08x will set to mode %d program id 0x%04x\n",addr,def,fname);
  Command(addr, cAMSW_FLASHDF, 2, fname, def );
  PrintRXDONE();
}



void AMSWcom::WriteDelay(unsigned int addr, unsigned short delay) {
  IOutput("Address 0x%08x will have a delay of 0x%04x (%2.2f us)\n",addr,delay,(float)delay*0.02);
  Command(addr,cAMSW_DELAYWR,1,delay);
  PrintRXDONE();

}


unsigned short AMSWcom::ReadDelay(unsigned int addr) {

  Command(addr,cAMSW_DELAYRD,0);
  unsigned short delay=Event[0];
  IOutput("Address 0x%08x has a delay of 0x%04x (%2.2f us)\n",addr,delay,(float)delay*0.02);
  PrintRXDONE();
  return Event[0];
}



void AMSWcom::GetStatus(unsigned int addr) {

  Command(addr,cAMSW_GETSTAT,0);

  memset(&NStat,0,sizeof(NStat));

  int cnt=0;
  NStat.Ver=Event[cnt++];
  NStat.Attr=Event[cnt++];
  NStat.SubdVer=Event[cnt++];
  NStat.LStime=Event[cnt++];
  NStat.MStime=Event[cnt++];
  NStat.NodeStat=Event[cnt++];
  NStat.LastEvt=Event[cnt++];
  NStat.NBuildErr=Event[cnt++];
  NStat.NAMSWerr=Event[cnt++];
  NStat.NFlashErr=Event[cnt++];
  NStat.ReplyStat=Event[cnt++];

}


void AMSWcom::GetSummary(unsigned int addr) {

  // note that the 4000 case is not taken into account as of now

  Command(addr,cAMSW_FLASHDIR,0);

  memset(FlashSummary,0,sizeof(FlashSummary));

  for (int i=0,j=0; i<8; i++) {
    FlashSummary[i].FileID=Event[j++];
    FlashSummary[i].AddrH=Event[j++];
    FlashSummary[i].EraseS=Event[j++];
    FlashSummary[i].Stat=Event[j++];
  }
  
}



void AMSWcom::CalibrateDac(unsigned int addr, unsigned short dac) {
  IOutput("DAC calibration of address %08x\n",addr);

  struct timeval tv1,tv2;
  gettimeofday(&tv1,0);
  double elapsed=0;
  

  Command(addr,cAMSW_CALDAC,1,dac);
  gettimeofday(&tv2,0);
  elapsed=(tv2.tv_sec-tv1.tv_sec)*1000+ (tv2.tv_usec-tv1.tv_usec)/1000;

  IOutput("Elapsed time is %6.2f seconds\n",elapsed/1000.);

  PrintRXDONE();
  unsigned short daccal[1024];

  ReadMemory(addr,0x1702,0x400,kDSP_PM,1);

  for (int i=0; i<1024; i++) daccal[i]=Event[i];



  char fname[100];
  sprintf(fname,"out%d.dac",dac);
  FILE *f=fopen(fname,"wt");
  for (int i=0; i<1024; i++) fprintf(f,"%d\n",daccal[i]);
  fclose(f);

}



void AMSWcom::Calibrate(unsigned int addr) {
  IOutput("Calibrating address 0x%08x\n",addr); 
  Command(addr,cAMSW_CALPED,0);
  PrintRXDONE();
}


void AMSWcom::Calibrate(unsigned int addr, unsigned short par) {
  IOutput("Calibrating address 0x%08x with par %d\n",addr,par);
  Command(addr,cAMSW_CALPED,1,par);
  PrintRXDONE();
}

int AMSWcom::Check_for_EPPUP(){
  do {                                                                                                          
    if( eppUp == false ) {                                                                                    
      if( Init( ) == ERROR ) {                                                                              
        FOutput( COL_STRONG "Unable to get control of the EPP: " COL_EVIDENT "%s\n", strerror(errno));    
        return -1000;                                                                                      
      }                                                                                                      
    }                                                                                                          
  } while ( 0 ); 
  return 0;
}

void AMSWcom::GetCalibration(unsigned int addr) {
  //CHECK_FOR_EPPUP;

    
  unsigned short iPed[1024];
  unsigned short iSig[1024];
  unsigned short iSigr[1024];
  unsigned short iSta[1024];
  
  // * read_pm( address, length, *data )
  ReadMemory(addr,CalParMem.Ped,0x400,CalParMem.MemPed,1);
  memcpy(iPed,Event,sizeof(iPed));

  ReadMemory(addr,CalParMem.SigL,0x400,CalParMem.MemSigL,1);
  memcpy(iSig,Event,sizeof(iSig));

  ReadMemory(addr,CalParMem.Stat,0x400,CalParMem.MemStat,1);
  memcpy(iSta,Event,sizeof(iSta));

  ReadMemory(addr,CalParMem.Sigr,0x400,CalParMem.MemSigr,1);
  memcpy(iSigr,Event,sizeof(iSigr));

  //  printf("siglfactor=%f\n",  CalParMem.SigLFactor);

  for(int i=0; i<1024; i++) {
      ped[i]=(double)(iPed[i])/CalParMem.PedFactor;
      sig[i]=(double)(iSig[i])/CalParMem.SigLFactor;
      sigr[i]=(double)(iSigr[i])/CalParMem.SigrFactor;
      sta[i]=(iSta[i]);
  }
}


SlaveMask AMSWcom::ReadSlaveMask(unsigned int addr) {
  IOutput("Reading slave mask for address 0x%08x\n",addr);

  SlaveMask mask;
  memset(&mask,0,sizeof(mask));

  Command(addr,cAMSW_MASKRD,0);
  PrintEvent();
  for (int id=0; id<16; id++) {
    for (int slave=0; slave<24; slave++) {
      mask.ID[id]|=(((Event[slave]&(1<<id))?1:0)<<slave);
    }
    if (mask.ID[id]) mask.Nmask=id+1;
    //printf("id=%d, mask=0x%x\n",id,mask.ID[id]);
  }

  //printf("there are %d masks\n", mask.Nmask);
  PrintRXDONE();

  if (EventSize&& sType==kINTER) ShowConnect(mask);  
  return mask;
}


void AMSWcom::WriteSlaveMask(unsigned int addr, unsigned short id, unsigned int mask) {

  unsigned short val1,val2;

  mask&=0xffffff;
  id&=0xff;
  val1=(mask>>16)+(id<<8);
  val2=mask&0xffff;

  printf("mask= 0x%04x 0x%04x\n", val1,val2); 

  WriteSlaveMask(addr,val1,val2);
}



void AMSWcom::WriteSlaveMask(unsigned int addr, unsigned short val1, unsigned short val2) {
  IOutput("Writing slave mask for address 0x%08x\n",addr);
  Command(addr,cAMSW_MASKLD,2,val1,val2);
  PrintRXDONE();
}





void AMSWcom::WriteBusyMask(unsigned int addr, unsigned int mask) {

  unsigned short val1,val2;

  mask&=0xffffff;
  val1=(mask>>16);
  val2=mask&0xffff;

  //printf("mask= 0x%04x 0x%04x\n", val1,val2); 

  WriteBusyMask(addr,val1,val2);
}


void AMSWcom::WriteBusyMask(unsigned int addr, unsigned short val1, unsigned short val2) {
  IOutput("Setting busy mask %04x %04x to address %08x\n", val1,val2, addr);
  Command(addr,cAMSW_BUSYLD,2,val1,val2);
  PrintRXDONE();
}



unsigned int AMSWcom::ReadBusyMask(unsigned int addr) {

  Command(addr, cAMSW_BUSYRD, 0);
  unsigned int mask=(Event[0]<<16)+Event[1];
  IOutput("Busy mask of address %08x is %08x\n", addr, mask);
  ShowTDRs(mask);

  PrintRXDONE(); 
  return (Event[0]<<16)+Event[1];
}



unsigned int AMSWcom::ReadBusyStat(unsigned int addr) {

  Command(addr, cAMSW_BUSYSTAT, 0);
  unsigned int stat=(Event[0]<<16)+Event[1];

  IOutput("Busy status of address %08x is %08x\n", addr, stat);

  ShowTDRs(stat);

  PrintRXDONE(); 
  return (Event[0]<<16)+Event[1];
}



void AMSWcom::SetMode(unsigned int addr, unsigned short mode) {
  IOutput("Setting processing mode to%s%s%s for address 0x%08x\n",(!mode)?" NO":"", (mode&1)?" RAW":"",(mode&2)?" RED":"",addr);
  Command(addr,cAMSW_CHMODE,1,mode);
  PrintRXDONE();

}


unsigned short AMSWcom::ReadMode(unsigned int addr) {

  Command(addr,cAMSW_RDMODE,0);
  
  unsigned short mode=Event[0];
  IOutput("Address 0x%08x is in the%s%s%s processing mode\n",addr,(!mode)?" NO":"",(mode&1)?" RAW":"",(mode&2)?" RED":"");
  PrintRXDONE();
  return mode;
}


void AMSWcom::EventReset(unsigned int addr) {
  IOutput("Clearing Event FIFO and last event number of address 0x%08x\n",addr);
  Command(addr,cAMSW_EVNTRST,0);
  PrintRXDONE();

}


unsigned short AMSWcom::GetLastEventN(unsigned int addr) {
  IOutput("Getting last event number from address 0x%08x\n",addr);

  Command(addr,cAMSW_EVNTNUM,0);
  PrintRXDONE();
  PrintEvent();
 
  return Event[0];
}


unsigned short AMSWcom::GetEvent(unsigned int addr) {
  IOutput("Getting event from address 0x%08x\n", addr);
  Command(addr,cAMSW_EVNTGET,0);
  PrintRXDONE();
  PrintEvent();
  return EventSize;
}

void AMSWcom::PrintEvent() {
  int length=EventSize;
  for (int i=0; i<length; i++) {
    if (i%8==0) IOutput("%5d: ",i);
    IOutput("%04x ",Event[i]);
    if ((i+1)%8==0) IOutput("\n");
  }
  //  IOutput("CRC= 0x%04x\n",node->GetCRC());  
  IOutput("\n");
}

int AMSWcom::CommandEPP(int args, unsigned short *params, int mode24) {
  int tt;
  if( (tt=Check_for_EPPUP()) !=0 ) return tt;

  int  i;


  
  // * initialize BM write
  bmwr_init ( );
  
  // send command
  for (int cnt=0; cnt<args; cnt++) outw(params[cnt], EPPDATA);

  // * do other stuff :P
  outb( ioPORT_RXAD(port), EPPADDRESS );
  outw( cBM_PORT_RXBUF(port),   EPPDATA );
  
  // * read out rx-status
  rx_status ( );
  
  // * start tx on this port
  // cases with more than 1 packet are also foreseen
  unsigned short txst=0x9000;
  unsigned short length=0;
  unsigned short nw=args;
  while (nw>0xfff) {
    tx_start ( cBM_PORT_TXBUF(port)+length, txst+0xfff );
    txst=0x8000;
    length+=0xfff;
    nw-=0xfff;

    RX_DONE=rx_done();

    //    printf("nw=%x length=%x txst=%x rxdone=%x\n",nw,length,txst,RX_DONE);
    //if ((RX_DONE & 0xE000) == 0x4000 ) if ((RX_DONE & 0x180) != 0) return RX_DONE;
  }

  txst|=0x2000;
  tx_start ( cBM_PORT_TXBUF(port)+length, txst+nw );
  
  EventSize=0;
  unsigned short len=0;
  do {
    RX_DONE=rx_done();
    if ( (RX_DONE & 0xE000) == 0x4000 ) {
      //printf("rx status=%04x\n", EPP_RX_DONE);      
      len=rx_len();
      
      if (len>0) {
	bmrd_init();
	for (i=0; i<len; i++) Event[EventSize+i]=inw(EPPDATA);
	EventSize+=len;

	if ((RX_DONE & cPORT_RXBIT_BC2)==0 ) {
	  outb( ioPORT_RXAD(port), EPPADDRESS );
	  outw( cBM_PORT_RXBUF(port),   EPPDATA );
	  rx_status();

	  tx_start(cBM_PORT_TXBUF(port),0x8000);
	  continue;
	}
      }
    } else {
      //printf("COMMAND: RX errors, status %04x", RX_DONE);
    }
  } while((len>0)&&((RX_DONE & cPORT_RXBIT_BC2)==0)); // we are waiting for 'end of block'

  if (EventSize==0) return 0;

  EventSize--;

  if (EventSize>0) {
    BuildStat=Event[EventSize-1];
    CRC=Event[EventSize]; 
    if (DEBUG) printf("EPP: crc= 0x%04x , DoFCS= 0x%04x\n", CRC, DoFCS());
    if (CRC!=DoFCS()) return (RX_DONE=-6);
  }

  else { BuildStat=0; CRC=0; }
  

  if (mode24) { 
    EventSize=Convert24()+1;
    Event[EventSize-1]=BuildStat;
    Event[EventSize]=CRC;
  }


  return 0;
}



int AMSWcom::CommandEPP(unsigned int addrl, unsigned char cmd, int args, ... ) {

  unsigned short addr=addrl & 0xffff;
  unsigned short addr2=addrl>>16;

  va_list ap;
  int cnt=0;

  if (args>(CMDSIZE-1)) { printf("too many args\n"); return -12345; }

  if (addr!=0xffff) {
    CmdArray[cnt++]=addr; // addr can be 0xYY3f or 0x4000 or ...
    if (addr2) CmdArray[cnt++]=addr2; // secondary address, useful if you use e.g. a Jinj.
  }
  
  // * send the command

  CmdArray[cnt++]=0x2E00+cmd;
  
  // * send eventual args
  va_start(ap,args);
  for(int i=0; i<args; i++) {
    unsigned short s =(unsigned short)va_arg(ap,int);
    CmdArray[cnt++]=s;
  }
  va_end( ap );

  return CommandEPP(cnt,CmdArray,(cmd==cAMSW_READPM));
}


void AMSWcom::Command_Lv1( int ntrig) {
  int cnt=0;
  while (1) {
    Command_Lv1();
    if (ntrig!=-1) {
      cnt++;
      usleep(100000);
      if (cnt>=ntrig) break;
    }
  }
}

int AMSWcom::Command_Lv1() {

  unsigned short flag;
  //  CHECK_FOR_EPPUP;
  int tt;
  if( (tt=Check_for_EPPUP())!=0) return tt;


  flag=busy_stat();
  //printf ( "\n Busy flag before sending LV1 - %02x \n", flag);

  lv1_start();

  flag=busy_stat();
  //printf ( " Busy flag right after LV1 - %02x \n", flag);

  return 0;
}




void AMSWcom::lv1_start(void) {

  outb(ioLV1_START,EPPADDRESS);
  outw(0x3, EPPDATA);

}

unsigned short AMSWcom::busy_stat(void) {

  outb(ioBUSY_STAT,EPPADDRESS);
  return inw(EPPDATA);

}



void AMSWcom::WriteMemory(unsigned int addr, unsigned short start, unsigned short length, int memtype) {
  switch (memtype) {
  case kDSP_PM:
    WritePM(addr,start,length);
    break;
  case kDSP_DM:
    WriteDM(addr,start,length);
    break;
  }
}



void AMSWcom::WritePM(unsigned int addrl, unsigned short start, unsigned short length) {

  unsigned short addr=addrl & 0xffff;
  unsigned short addr2=addrl>>16;

  unsigned short command[EVENTSIZE+5];


  unsigned int cnt=0;

  if (addr!=0xffff) {
    command[cnt++]=addr; // addr can be 0xYY3f or 0x4000 or ...
    if (addr2) command[cnt++]=addr2; // secondary address, useful if you use e.g. a Jinj.
  }

  //  command[cnt++]=addr;
  command[cnt++]=0x2e00+cAMSW_WRITEPM;
  command[cnt++]=length;
  command[cnt++]=start;

  // data are stored the Event array
  //printf("before loop\n");
  for (int i=0; i<length*3/2; i++) command[cnt++]=Event[i];
  //printf("after loop\n");
  //for (int i=0; i<cnt; i++) printf("i=%4d  command=0x%04x\n",i, command[i]);
  Command(cnt,command,0);
  //printf("after command\n");

}


void AMSWcom::WriteDM(unsigned int addrl, unsigned short start, unsigned short length) {

  unsigned short addr=addrl & 0xffff;
  unsigned short addr2=addrl>>16;

  unsigned short command[EVENTSIZE+5];


  unsigned int cnt=0;

  if (addr!=0xffff) {
    command[cnt++]=addr; // addr can be 0xYY3f or 0x4000 or ...
    if (addr2) command[cnt++]=addr2; // secondary address, useful if you use e.g. a Jinj.
  }
  //  command[cnt++]=addr;
  command[cnt++]=0x2e00+cAMSW_WRITEDM;
  command[cnt++]=0x4000 | length;
  command[cnt++]=start;

  // data are stored the Event array

  for (int i=0; i<length; i++) command[cnt++]=Event[i];
  
  Command(cnt,command,0);

  // some crc checks should be added there...
}


void AMSWcom::Ping(unsigned int addr, int nval) {

  for (int cnt=0; cnt<nval; cnt++) Event[cnt]=cnt;
  
  Ping(addr,nval);
  
  PrintRXDONE();
  PrintEvent();

}
void AMSWcom::Ping(unsigned int addrl, unsigned short length) {

  unsigned short addr=addrl & 0xffff;
  unsigned short addr2=addrl>>16;

  unsigned short command[EVENTSIZE+5];


  unsigned int cnt=0;

  if (addr!=0xffff) {
    command[cnt++]=addr; // addr can be 0xYY3f or 0x4000 or ...
    if (addr2) command[cnt++]=addr2; // secondary address, useful if you use e.g. a Jinj.
  }
  //  command[cnt++]=addr;
  command[cnt++]=0x2e00+cAMSW_PING;

  // data are stored the Event array

  for (int i=0; i<length; i++) command[cnt++]=Event[i];
  
  Command(cnt,command,0);

}



void AMSWcom::Ping(ushort addr) {

  Command(addr,cAMSW_PING,0);

}





void AMSWcom::FlashWrite(unsigned int addrl, unsigned short length) {

  unsigned short addr=addrl & 0xffff;
  unsigned short addr2=addrl>>16;

  unsigned short command[EVENTSIZE+5];
 
  unsigned int cnt=0;

  if (addr!=0xffff) {
    command[cnt++]=addr; // addr can be 0xYY3f or 0x4000 or ...
    if (addr2) command[cnt++]=addr2; // secondary address, useful if you use e.g. a Jinj.
  }

  unsigned short start=cnt;
  command[cnt++]=0x2e00+cAMSW_FLASHWR;

  // data are stored in the Event array

  for (int i=0; i<length; i++) command[cnt++]=Event[i];

  unsigned short fcs=DoFCS(&command[start],cnt-start);
  command[cnt++]=fcs;
  //printf("fcs=%x\n",fcs);
  //fcs=DoFCS(&command[start],cnt-start);
  //printf("fcs=%x\n",fcs);

  //for (int i=0; i<cnt; i++) printf("%04x\n",command[i]);

  Command(cnt,command,0);

}


void AMSWcom::WriteQList(unsigned int addrl, QList *qlist) {

  unsigned short addr=addrl & 0xffff;
  unsigned short addr2=addrl>>16;

  unsigned short command[EVENTSIZE+5];
 
  unsigned int cnt=0;

  if (addr!=0xffff) {
    command[cnt++]=addr; // addr can be 0xYY3f or 0x4000 or ...
    if (addr2) command[cnt++]=addr2; // secondary address, useful if you use e.g. a Jinj.
  }

  command[cnt++]=0x2e00+cAMSW_QLISTWR;
  unsigned short sizeloc=cnt;
  command[cnt++]=0x4000; // DM segment header 1st word (will contain the segment size)
  command[cnt++]=0x2000; // DM segment header 2nd word

  unsigned short start=cnt; // starting from next command, we will need to compute a crc
  unsigned short cmdcnt=qlist->GetCmdcnt();
  command[cnt++]=cmdcnt; // number of commands to add. For now, we say only 1

  QCommand *qcmd=0;
  for (int i=0; i<cmdcnt; i++) {
    if (i==0) qcmd=qlist->GetQfirst();
    for (int j=0; j<5; j++) command[cnt++]=qcmd->GetCommand(j);
    qcmd=qcmd->GetNext();
    if (qcmd==0) break; // security... should only happend at the last loop...
  }
  
  // now we compute the CRC

  command[sizeloc]+=cnt-sizeloc-1; // we write the segment size

  unsigned short fcs=DoFCS(&command[start],cnt-start);
  command[cnt++]=fcs;
  
  //printf("fcs=%x\n",fcs);

  for (int i=0; i<cnt; i++) printf("%04x ", command[i]);  

  Command(cnt,command,0);

}


void AMSWcom::ReadQList(unsigned int addrl) {
  char typech[4][20]={"LeCroy\0", "AMSWire\0", "Memory op\0", "IO op\0"};

  unsigned int freq=0, time=0;
  int executed=0, attribute=0, first=0, last=0;
  unsigned short w0=0, w1=0, r0=0, r1=0, offset=0, type=0;
  Command(addrl,cAMSW_QLISTRD,0);
  unsigned length=GetEventSize();
  
  QEvent *qevent=new QEvent();
  qevent->GetQListData(Event,length);
  if(sType==kINTER)qevent->Print();
  if(qevent) delete qevent;

}


void AMSWcom::ReadDM(unsigned int addr, unsigned short start, unsigned short length) {
  IOutput("Reading Data Memory of address 0x%08x, at 0x%04x and length 0x%04x\n", addr, start, length);
  Command(addr,cAMSW_READDM,2,(0x4000 | length),start);
  PrintRXDONE();
  PrintEvent();
}



void AMSWcom::ReadMemory(unsigned int addr, unsigned short start, unsigned short length, int memtype, int mode24) {
  switch (memtype) {
  case kDSP_PM:
    ReadPM(addr,start,length, mode24);
    break;
  case kDSP_DM:
    ReadDM(addr,start,length);
    break;
  }
}


void AMSWcom::ReadPM(unsigned int addr, unsigned short start, unsigned short length, int mode24) {
  IOutput("Reading Program Memory of address 0x%08x, at 0x%04x and length 0x%04x\n", addr, start, length);

  Command(addr,cAMSW_READPM,3,length,start, mode24);
  PrintRXDONE();
  PrintEvent();
  IOutput("event size=%d\n", EventSize);
}



void AMSWcom::LecroyWrite(ushort addr, ushort bus) {

  Command(addr, cAMSW_LRSWR, 1, bus);

}


uint AMSWcom::LecroyRead(unsigned int addr, ushort bus, ushort lrcommand, ushort parameter) {
  // here I chose the simplest syntax, i.e. one LR command per AMSW transaction.

  ushort result=0;
  int ok=0;
  unsigned short cnt1=0, cnt2=0;
  unsigned int lerror=0xffff;

  do {
    Command(addr, cAMSW_LRSRD, 5, 1, 0, bus, lrcommand, parameter);
    result=CheckLecroyReply(bus, lrcommand, parameter, ok);

    if (ok==0) {
      cnt1++;
      if (cnt1>5) {
	LecroyWrite(addr,bus);
	cnt2++;
      }
    }
  } while ((ok==0) && cnt1<11);

  lerror=((cnt2 & 0xff)<<8) + (((cnt1) & 0xff));
  //if (lerror) printf("lecroy: error=%x\n",lerror);
  return (ok)?result:((lerror<<16) | result);
}


ushort AMSWcom::CheckLecroyReply(ushort bus, ushort lrcommand, ushort parameter, int &ok) {
  int test0=(Event[0]==1 && Event[1]==0);
  int test1=(bus==Event[2] && lrcommand==Event[3] && parameter==Event[4]);
  int test2=((Event[5]^lrcommand)==0x8000);
  ok=(test0 && test1 && test2);
  return Event[6];
}


// void AMSWcom::LecroyRead(ushort addr, ushort ncmd, ushort *delay, ushort *bus, ushort *lrcommand, ushort *parameter) {
//   ushort cnt=0;
//   // delay: in 10 ms units.
//   if (addr!=0xffff) CmdArray[cnt++]=addr; // addr can be 0xYY3f or 0x4000 or ...
//   CmdArray[cnt++]=0x2e00+cAMSW_LRSRD;
//   CmdArray[cnt++]=ncmd;

//   for (int i=0; i<ncmd; i++) {
//     CmdArray[cnt++]=delay[i];
//     CmdArray[cnt++]=bus[i];
//     CmdArray[cnt++]=lrcommand[i];
//     CmdArray[cnt++]=parameter[i];
//   }

//   Command(cnt, CmdArray, 0);

//   // still to be understood is the reply format of such a command...

// }


void AMSWcom::LecroyRead(unsigned int addrl, QList *qlist) {
  // here we send a set of LR commands, using a QList as the command container
  // the data will be read out using the the QEvent objects

  unsigned short addr=addrl & 0xffff;
  unsigned short addr2=addrl>>16;

  unsigned short command[EVENTSIZE+5];
 
  unsigned int cnt=0;

  if (addr!=0xffff) {
    command[cnt++]=addr; // addr can be 0xYY3f or 0x4000 or ...
    if (addr2) command[cnt++]=addr2; // secondary address, useful if you use e.g. a Jinj.
  }

  command[cnt++]=0x2e00+cAMSW_LRSRD;



  unsigned short cmdcnt=qlist->GetCmdcnt();
  command[cnt++]=cmdcnt; // number of commands to add.

  QCommand *qcmd=0;
  for (int i=0; i<cmdcnt; i++) {
    if (i==0) qcmd=qlist->GetQfirst();
    //    for (int j=0; j<5; j++) command[cnt++]=qcmd->GetCommand(j);
    command[cnt++]=qcmd->GetFreq(); // pay attention that the freq in QList has 8 more bits in the header, for loooooong delay commands, which is not really the goal of the ReadLecroy command.
    command[cnt++]=qcmd->GetLRBus();
    command[cnt++]=qcmd->GetW0();
    command[cnt++]=qcmd->GetW1();
    qcmd=qcmd->GetNext();
    if (qcmd==0) break; // security... should only happend at the last loop...
  }
  

  //  for (int i=0; i<cnt; i++) printf("%04x ", command[i]);  

  Command(cnt,command,0);

}






unsigned short AMSWcom :: rx_status (  )
{
	outb( ioPORT_RXST(port), EPPADDRESS );
	return inw( EPPDATA);
}


unsigned short AMSWcom :: rx_done ( )
{

  unsigned short m;
  
  switch (port) {
  case 0:
    outb(ioPORT0_RXST,EPPADDRESS);
    break;
  case 1:
    outb(ioPORT1_RXST,EPPADDRESS);
    break;
  case 2:
    outb(ioPORT2_RXST,EPPADDRESS);
    break;
  case 3:
    outb(ioPORT3_RXST,EPPADDRESS);
    break;
  default:
    return 0;
  }
  

  struct timeval tv1,tv2;
  gettimeofday(&tv1,0);
  double elapsed=0;
  do {
    m=inw(EPPDATA);
    if (cPORT_RXBIT_DONE & m) break;   // wait untill done=1
    gettimeofday(&tv2,0);
    elapsed=(tv2.tv_sec-tv1.tv_sec)*1000+ (tv2.tv_usec-tv1.tv_usec)/1000;
  } while (elapsed<TimeOut); // TimeOut in ms

  gettimeofday(&tv2,0);
  //printf("Elapsed time: %ld ms\n", (tv2.tv_sec-tv1.tv_sec)*1000+ (tv2.tv_usec-tv1.tv_usec)/1000);

  return m;

}


unsigned short AMSWcom :: rx_len (  )
{
	outb( ioPORT_RXAD(port), EPPADDRESS );
	return (inw( EPPDATA ) & 0xFFF);
}

void AMSWcom :: tx_start ( unsigned short addr, unsigned short stat )
{
	outb( ioPORT_TXAD(port),  EPPADDRESS);
	outw( addr,	EPPDATA);

	outb( ioPORT_TXST(port),  EPPADDRESS);
	outw( stat,			   EPPDATA);
}


void AMSWcom :: bmrd_init (  )
{
	outb( ioBM_ADDR_RD,	   EPPADDRESS );
	outw( cBM_PORT_RXBUF(port),  EPPDATA );
	outb( ioBM_DATA_RD,	   EPPADDRESS );
}


void AMSWcom :: bmwr_init (  )
{
	outb( ioBM_ADDR_WR,	  EPPADDRESS );
	outw( cBM_PORT_TXBUF(port), EPPDATA );
	outb( ioBM_DATA_WR,	  EPPADDRESS );
}


void AMSWcom :: crcrd_init (  )
{
	outb(ioBM_CRC_RD, EPPADDRESS);
	outw(0x0000, EPPDATA);
}

unsigned short AMSWcom :: crcrd_end (  )
{
	unsigned short crc;
	outb( ioBM_CRC_RD, EPPADDRESS);
	crc = inw(EPPDATA);
	if( crc != 0 )
		FOutput( COL_STRONG "Error reading CRCrd " COL_EVIDENT "0x%04x != 0\n", crc );
	return crc;
}

void AMSWcom :: crcwr_init (  )
{
	outb(ioBM_CRC_WR, EPPADDRESS);
	outw(0x0000, EPPDATA);
}


unsigned short AMSWcom :: crcwr_end (  )
{
	unsigned short crc;
	outb(ioBM_CRC_WR, EPPADDRESS);
	crc = inw(EPPDATA);
	//	if( crc != 0 )
	//		FOutput( COL_STRONG "Error reading CRCwr " COL_EVIDENT "0x%04x != 0\n", crc );
	return crc;
}





unsigned short AMSWcom::DoFCS(unsigned char *data, int size) {

  // from Kounine + Koutsenko documentation, p30

  unsigned short fcs=0xffff;

  for(int i=0; i<size; i++) fcs=FCS_Table[(fcs>>8)^data[i]]^(fcs<<8);

  //  printf("DoFCS = %04x\n",fcs);

  return fcs;
}


unsigned short AMSWcom::DoFCS(unsigned short *data, int size) {

  // from Kounine + Koutsenko documentation, p30

  //printf("DoFCS: data[0]=%x\n",data[0]);

  unsigned short fcs=0xffff;
  unsigned char val;

  for(int i=0; i<size; i++) { 
    val=data[i]>>8;
    fcs=FCS_Table[(fcs>>8)^val]^(fcs<<8);
    val=data[i] & 0xff;
    fcs=FCS_Table[(fcs>>8)^val]^(fcs<<8);
  }

  //  printf("DoFCS = %04x\n",fcs);

  return fcs;
}



unsigned short AMSWcom::DoFCS(int size) {

  // from Kounine + Koutsenko documentation, p30

  unsigned short fcs=0xffff;
  unsigned char val;

  for(int i=0; i<size; i++) { 
    val=Event[i]>>8;
    fcs=FCS_Table[(fcs>>8)^val]^(fcs<<8);
    val=Event[i] & 0xff;
    fcs=FCS_Table[(fcs>>8)^val]^(fcs<<8);
  }

  //  printf("DoFCS = %04x\n",fcs);

  return fcs;
}

unsigned short AMSWcom::DoFCS() {
  return DoFCS(EventSize);
}


void AMSWcom::PrepareFCSTable() {

  // from Kounine + Koutsenko documentation, p30

  unsigned short const g=0x1021;
  unsigned short d,i,k,fcs;

  // prepare lookup table

  for(i=0; i<256; i++) {
    fcs=0;
    d=i<<8;
    for(k=0; k<8; k++) {
      if ((fcs^d) & 0x8000) fcs=(fcs<<1)^g;
      else fcs<<=1;
      d<<=1;
    }
    FCS_Table[i]=fcs;
  }
}





void AMSWcom::ShowConnect(SlaveMask mask) {
  //const int config[24]={0,4,8,0xc,0x10,0x14,0x16,0x12,0xe,0xa,6,2,  1,5,9,0xd,0x11,0x15,0x17,0x13,0xf,0xb,7,3};  
  IOutput("\n Summary:\n");
  //  const char on[2][5]={"[ ]\0","[X]\0"};
  
  for (int id=0; id<mask.Nmask; id++) {
    unsigned int connect=mask.ID[id];
    IOutput("Mask ID=%02x : 0x%06x\n",id,connect);

//    for (int j=0; j<2; j++) {
//       for (int i=0; i<12; i++) {
//      //       if (connect&(1<<config[j*12+i])) {
//      //      textcolor(BRIGHT,GREEN,BLACK);
//      //      } else {
//      //        textcolor(BRIGHT,RED,BLACK);
//      //      }
//      IOutput(" %3x:%s", config[j*12+i],on[(connect&(1<<config[j*12+i]))?1:0]);
//       }
//       IOutput("\n");
//     }
//     //  textcolor(BRIGHT,WHITE,BLACK);

    ShowTDRs(connect);

  }
}


void AMSWcom::ShowTDRs(unsigned int mask) {
  const int config[24]={0,4,8,0xc,0x10,0x14,0x16,0x12,0xe,0xa,6,2,  1,5,9,0xd,0x11,0x15,0x17,0x13,0xf,0xb,7,3};  
  const char on[2][5]={"[ ]\0","[X]\0"};

  IOutput("\n Summary:\n");

  for (int j=0; j<2; j++) {
    for (int i=0; i<12; i++) IOutput(" %3x:%s", config[j*12+i],on[(mask&(1<<config[j*12+i]))?1:0]);
    IOutput("\n");
  }
}




void AMSWcom::PrintSummary( unsigned int address) {

  GetSummary(address);
  PrintRXDONE();
  if (!(EventSize)) return;
  IOutput("Flash Summary of address 0x%08x:\n", address);
  IOutput("--------------------------------------------\n");
  IOutput("| Sector | File Id | AddrH | EraseS | Stat |\n");
  IOutput("--------------------------------------------\n");
  
  for (int sec=0; sec<8; sec++) {
    //    IOutput("Sector   File Id   AddrH   EraseS   Stat\n");
    IOutput("|   %d    |   %04x  |  %04x |  %04x  | %04x |\n",sec,GetSummary(sec).FileID,GetSummary(sec).AddrH,GetSummary(sec).EraseS, GetSummary(sec).Stat);
  }
  IOutput("--------------------------------------------\n");
  //IOutput("buildstat = 0x%04x\n", node->GetBuildStat());

  PrintBuildStat(GetBuildStat());

}

void AMSWcom::PrintBuildStat(unsigned short e1) {

  char string[200];

  sprintf(string, "Slave ID %d: ",(e1 & 0x1f));
  //  writeDAQMessg(string);
      
  sprintf(string,"%s%snode is building%s%s%s events",string,(e1 & (1<<5))?"CDP ":"",(e1 & (1<<6))?" RAW":"", (e1 & (1<<7))?" CMP":"", (e1 & ((1<<6)+(1<<7)))?"":" no");
  IOutput("%s\n",string);
      
  if (e1 & (1<<15)) {
    sprintf(string,"DATA");
    if (e1 & (1<<14)) sprintf(string,"%s CRC error",string);
    if (e1 & (1<<13)) sprintf(string,"%s assembly error",string);
    if (e1 & (1<<12)) sprintf(string,"%s AMSW error",string);
    if (e1 & (1<<11)) sprintf(string,"%s timeout",string);
    if (strlen(string)>0) IOutput("%s\n",string);  
  } else {
    sprintf(string,"");
    if (e1 & (1<<14)) sprintf(string,"%s END",string);
    if (e1 & (1<<13)) sprintf(string,"%s ABORT",string);
    if (e1 & (1<<12)) sprintf(string,"%s ERROR",string);
    if (e1 & (1<<11)) sprintf(string,"%s NEXT",string);
    if (strlen(string)>0) IOutput("%s\n",string);  
  }

}


void AMSWcom::PrintNodeStatus( unsigned int addr) {
  static char prgtyp[2][20]={"ROM mon\0","DAQ\0"};
  static char nodtyp[2][20]={"CDP\0","CDDC\0"};
  static char evbtyp[7][20]={"CDDC\0","TDR\0","UDR\0","RDR\0","EDR\0","SDR\0","LV1\0"};
  static char error[7][50]={"Program selftest\0","Data memory test\0","Program memory test\0",
                            "Buffer memory test\0","FLASH\0","AMSWire\0","Event building\0"};

  char string[200];
  unsigned short e1=0;
  unsigned short ver=0, attr=0, subdver=0, lastevt=0, subdsubver=0;
  unsigned short LStime=0, MStime=0;
  double times=0, timem=0, timeh=0, timed=0;

  GetStatus(addr);
  PrintRXDONE();

  ver=GetNodeStatus().Ver;
  sprintf(string,"Program version date %02d.%02d.%04d", (0x00ff & ver), (0x0f00 & ver)>>8, 2000+(ver>>12) );
  IOutput("%s\n",string);

  attr=GetNodeStatus().Attr;
  sprintf(string,"Program: %s   Node: %s   Evtbld: %s   Number of ports: %d",prgtyp[attr>>12],nodtyp[((attr & 0x0f00)>>8)-1],evbtyp[(attr & 0x00f0)>>4], attr & 0x000f);
  IOutput("%s\n",string);

  subdver=0xff1f & GetNodeStatus().SubdVer;
  subdsubver=(0xe0 & GetNodeStatus().SubdVer)>>5;
  sprintf(string,"Subdet version: %04x",subdver);
  IOutput("%s\n",string);


  LStime=GetNodeStatus().LStime;
  MStime=GetNodeStatus().MStime;


  lastevt=GetNodeStatus().LastEvt;
  sprintf(string,"Last event number assigned: %d\n",lastevt);
  IOutput("%s\n",string);



  sprintf(string,"%s (%s) running %s monitor of %02d.%02d.%04d",nodtyp[((attr & 0x0f00)>>8)-1],evbtyp[(attr & 0x00f0)>>4], prgtyp[attr>>12],(0x00ff & ver), (0x0f00 & ver)>>8, 2000+(ver>>12) );
  IOutput("%s\n",string);

  sprintf(string,"%s (%s) running subdet program of %02d.%02d.%04d, version %x",nodtyp[((attr & 0x0f00)>>8)-1],evbtyp[(attr & 0x00f0)>>4],(0x00ff & subdver), (0x0f00 & subdver)>>8, 2000+(subdver>>12) , subdsubver);
  IOutput("%s\n",string);

  IOutput("\n");
  PrintBuildStat(GetNodeStatus().ReplyStat);

  sprintf(string, "GET STATUS: BuildERR = %d, AMSW_ERR = %d, FLASH_ERR = %d, Last event = %d",GetNodeStatus().NBuildErr , GetNodeStatus().NAMSWerr, GetNodeStatus().NFlashErr, lastevt);
  IOutput("%s\n",string);

  e1=GetNodeStatus().NodeStat;
  //  IOutput("nodestat=%04x\n",e1);
  for (int er=0; er<7; er++) if (e1 & (1<<(8+er))) {
    sprintf(string,"%s errors",error[er]);
    IOutput("%s\n",string);
  }


  //IOutput("LS time= 0x%04x, MS time= 0x%04x\n", LStime, MStime);
  times=(LStime+(MStime<<16))*0.01;
  timem=times/60.;
  timeh=timem/60.;
  timed=timeh/24.;



  sprintf(string,"Started");
  if (!(timed<1)) sprintf(string,"%s %d days",string, (int) timed);
  if (!(timeh<1)) sprintf(string,"%s %d hours",string, (int)timeh%24);
  if (!(timem<1)) sprintf(string,"%s %d minutes",string, (int)timem%60);
  if (!(times<1)) sprintf(string,"%s %d seconds",string, (int)times%60); 
  sprintf(string,"%s ago\n",string);

  //  sprintf(string,"Started %.2lf seconds ago",(timed<1)?timed); // 10 ms ticks
  IOutput("%s\n",string);

}


int AMSWcom::SaveFlashFile( char *name) {

  unsigned short cnt=0;

  unsigned short nseg=Event[0] & 0xf;
  unsigned short seglen=0, start=0, flag=0;

  char filename[200];
  sprintf(filename,"%s",(name)?name:"file.dat");

  FILE *out=fopen(filename,"wt");
  if (out==0) { printf("Sorry, file problem...\n"); return 1; }

  fprintf(out,"%04x\n",Event[0]);

  cnt=1;
  for (int i=0; i<nseg; i++) {
    seglen=Event[cnt];
    cnt++;
    start=Event[cnt]; 
    cnt++;
 
    fprintf(out,"%04x ",seglen);
    fprintf(out,"%04x\n",start);

    flag=seglen & 0xc000; // flag=0: PM else DM
    seglen&=0x3fff;

    if (!flag) seglen=3*seglen/2;
    for (int i=0; i<seglen; i++) { 
      fprintf(out,"%04x\n",Event[cnt]);
      cnt++;
    }
  }



  fclose(out);

  return 0;
}

int AMSWcom::CreateFlashFile(unsigned int amswaddr) {

  char file_par[] = "file.par";
  char file_dat[] = "file.dat";

  FILE *in, *out;
  char chain[MAX_STRN];
  unsigned short fname, fn, flag, length, page, addr, nseg, slen, saddr, nw;

  if( (in = fopen(file_par, "r")) == NULL) {
    FOutput("File create: input file %s open error\n", file_par);
    return -1;
  }

  if( (out = fopen(file_dat, "w")) == NULL) {
    FOutput ("File create: output file %s open error\n", file_dat);
    return -1;
  }


  if ((fgets(chain, MAX_STRN, in)) != NULL) {               // Read file name
    stringbuf sb;
    sb.sputn(chain,strlen(chain));
    string mystr=sb.str();
    istringstream iss(mystr,istringstream::in);
    iss >> hex >> fn;
  }
  else
    return -1;
  
  fname = fn;
  
  fprintf (out, "%04x\n",fname);
  nseg = fname & 0xF;                                       // number of segments
  
  for (int i=0; i<nseg; i++) {

    if(fgets(chain, MAX_STRN, in) != NULL) {               // Read segment parameters
      //  FOutput ("0: %04x\n", fn);
      //sscanf(string, "%4x %4x", &slen, &saddr);             // Why fn gets overwriten here?
                                                            // because slen & saddr are short
                                                            // if they were long, there wouldn't
                                                            // be a problem (sscanf() definition).
      //  printf ("1: %04x %04x, %04x\n", fn, slen, saddr);

      stringbuf sb;
      sb.sputn(chain,strlen(chain));
      string mystr=sb.str();
      istringstream iss(mystr,istringstream::in);
      iss >> hex >> slen;
      iss >> hex >> saddr;

      flag   = slen  & 0xC000;
      length = slen  & 0x3FFF;
      page   = saddr & 0xC000;
      addr   = saddr & 0x3FFF;

      if ((flag==0) && (addr==0) && (i!=nseg-1)) {
        FOutput ("File create: segm %d (%04x, %04x) must be the last",i, slen, saddr);
        return -1;
      }

      if (flag == 0) { 
        // Program memory - 3B wide
        FOutput("ReadPM, addr=%04x, length=%04x\n",saddr,length);
        ReadPM(amswaddr, saddr,length,0);
        //nw = command_pm_read(saddr,length,port,tdr);
        nw=GetEventSize();
        FOutput ("file create, nw = %d\n", nw);
        if (nw>0) {
          nw--;                                          // strip Build STAT

          if (length & 1) {                                 // length must be even
            FOutput ("File create: PM segment error - odd length");
            //writeDAQMessg(string);
            return -1;
          }

          // last word is for CRC - must be zero(NOP)
          //FOutput("dd[nw-1]=0x%04x  dd[nw-2]=0x%04x  nw=0x%04x\n",Jinf->Event[nw-1],Jinf->Event[nw-2],nw);
          if (Event[nw-1] != 0) {                              // last word is for CRC
            FOutput ("File create: PM segment error - last word is not 0\n");
//          return -1;
          }

          fprintf (out, "%04x %04x\n",slen,saddr);          // start writing a segment
          //crcwr_init();
          //bmwr_init(cBM_PORT1_TXBUF);
          for (int j=0; j<nw-1; j++) {
          //  outw(dd[j],EPPDATA);                            // write data
            fprintf (out, "%04x\n",Event[j]);
          }

          // now we need to write the CRC
          fprintf(out,"%04x\n",DoFCS(nw-1));


        } else {                                            // CRC error in the AMSW packet
          FOutput ("File create: AMSW error\n");
          //writeDAQMessg(string);
          return -1;
        }
         
      } else {                                              // Data memory - 2B wide
        ReadDM(amswaddr,saddr,length);
        //nw = command_dm_read(saddr,length,port,tdr);
        nw=GetEventSize();
        //nw -= 2;                                            // strip Build STAT & CRC
        if (nw>0) {
          // CRC check
          slen += 1;
          
          fprintf (out, "%04x %04x\n",slen,saddr);          // start writing a segment
          for (int j=0; j<nw; j++) {
            //outw(dd[j],EPPDATA);                            // write data
            fprintf (out, "%04x\n",Event[j]);
          }
          
          fprintf(out,"%04x\n",DoFCS());

        } else {                                            // CRC error in the AMSW packet
          FOutput ("File create: AMSW error\n");
          //writeDAQMessg(string);
          return -1;
        }

      }

    } else {
      FOutput ("File create: segment %x has problems\n", i);
      //writeDAQMessg(string);
      return -1;
    }
    
  }

  FOutput ("File create: file ID %04x finished OK\n", fname);
  //writeDAQMessg(string);

  fclose(in);
  fclose(out);


}


int AMSWcom::WriteFlashFile(unsigned int amswaddr, char *name) {

  FILE *in;
  unsigned short fname, m, slen, saddr, nseg, flag, length, page, nw, fcs, txst, addr;
  char strng[MAX_STRN];

  if( (in = fopen(name, "r")) == NULL) {
    printf("Error while opening file %s\n",name);
    return 1;
  }

  if ((fgets(strng, MAX_STRN, in)) != NULL)                // Read file name
    sscanf(strng, "%4x", &m);
  else
    return -1;

  fname = m;
  nseg = fname & 0xF; 

  int cnt=0;
  Event[cnt]=fname;
  cnt++;
 
  for (int i=0; i<nseg; i++) {
    if(fgets(strng, MAX_STRN, in) != NULL) {               // Read segment parameters
      sscanf(strng, "%4x %4x", &slen, &saddr);
      Event[cnt]=slen;
      cnt++;
      Event[cnt]=saddr;
      cnt++;
      flag   = slen  & 0xC000;
      length = slen  & 0x3FFF;
      addr   = saddr & 0x3FFF;

      FOutput (" Segment %d, slen = %04x, saddr = %04x\n", i, slen, saddr);

      if ((flag==0) && (addr==0) && (i!=nseg-1)) {
        FOutput("FLASH write: segm %d (%04x, %04x) must be the last",i, slen, saddr);
        return 1;
      }

      if (length == 0) {
        FOutput("FLASH write: segm %d, invalid length = %d",i, slen);
        return 1;
      }

      if (flag == 0) {                                      // Program memory - 3B wide
        //      FOutput("Program memory...\n");
        if ((length%2) != 0) {
          FOutput("FLASH write: segm %d, invalid length = %d",i, slen);
          return 1;
        }

        length = 3 * (length/2);
        for (int j=0; j<length; j++) {
          if(fgets(strng, MAX_STRN, in) != NULL) {         // Read a word
            sscanf(strng, "%4x", &(Event[cnt]));
            cnt++;
          } else {
            FOutput("FLASH write: no data!");
            return 1;
          }
        }

      } else {                                              // Data memory - 2B wide
        //FOutput("Data memory...\n");
        for (int j=0; j<length; j++) {
          if(fgets(strng, MAX_STRN, in) != NULL) {         // Read a word
            sscanf(strng, "%4x", &(Event[cnt]));
            cnt++;
          } else {
            FOutput("FLASH write: no data!");
            return 1;
          }
        }
        
      }

    } else {
      FOutput("File write: segment %x has problems\n", i);
      return 1;
    }
  }


  fclose(in); 

  FlashWrite(amswaddr,cnt);
  PrintRXDONE();

  return 0;
}









void AMSWcom::SaveCalibration(unsigned int addr ,char * fname) {

  IOutput("Collecting and saving data for address 0x%08x\n",addr);



  GetCalibration(addr);

  FILE *calfil;
  if(fname)
    calfil=fopen(fname,"wt");
  else  calfil=fopen("calib.cal","wt");
  if (calfil==NULL) {
    FOutput("Error: file could not be created, perhaps the data dir does not exist ?\n");
    return;
  }

  for(int i=0; i<16; i++) fprintf(calfil,"%d,   %d,  %d\n",i,0,0); // to keep compatibilty with standard cal files

  for(int i=0; i<1024; i++) {
    fprintf(calfil,"%4d %2d %2d %4.2f %4.2f %4.2f %4.2f %4d\n",1+i, 1+i/64, 1+i%64, ped[i], sigr[i], sig[i],0.0, sta[i]);
    IOutput("%4d %2d %2d %4.2f %4.2f %4.2f %4.2f %4d\n",1+i, 1+i/64, 1+i%64, ped[i], sigr[i], sig[i],0.0, sta[i]);

  }
  fclose(calfil); 
  if(fname)IOutput("%s saved\n",fname);
  FOutput("calib.cal saved\n");
}




int AMSWcom::ProcessHexCommand(unsigned int addr,ushort cmd,int nparam, ushort * param){
  switch (cmd) {
  case cAMSW_SLAVTST:
    SlaveTest(addr);
    break;
  case cAMSW_EVNTGET:
    GetEvent(addr);
    break;
  case cAMSW_MASKLD:
    WriteSlaveMask(addr,param[0], (unsigned short)param[1]);
    break;
  case cAMSW_MASKRD:
    ReadSlaveMask(addr);
    break;
  case cAMSW_PED:
    SaveCalibration(addr);
    break;
  case cAMSW_READPM:
    ReadPM(addr,param[0],param[1],0);
    break;
  case cAMSW_READDM:
    ReadDM(addr,param[0],param[1]);
    break;
  case cAMSW_EVNTNUM:
    GetLastEventN(addr);
    break;
  case cAMSW_EVNTRST:
    EventReset(addr);
    break;
  case cAMSW_RDMODE:
    ReadMode(addr);
    break;
  case cAMSW_CHMODE:
    SetMode(addr,param[0]);
    break;
  case cAMSW_CALRED:
    SaveCalibration(addr,"caldone.txt");
    break;
  case cAMSW_CALPED:
    Calibrate(addr,0);
    break;
  case cAMSW_CALDAC:
    CalibrateDac(addr,param[0]);
    break;
  case cAMSW_DELAYRD:
    ReadDelay(addr);
    break;
  case cAMSW_DELAYWR:
    WriteDelay(addr,param[0]);
    break;
  case cAMSW_FLASHLD:
    FlashLoad(addr,param[0]);
    break;
  case cAMSW_FLASHRD:
    FlashRead(addr,param[0]);
    break;
  case cAMSW_FLASHDF:
    FlashDefault(addr,param[0],param[1]);
    break;
  case cAMSW_PING:
    Ping(addr,param[0]);
    break;
  case cAMSW_FLASHWR:
    WriteFlashFile(addr,"file.dat");
    break;
  case cAMSW_FLASHERASE:
    FlashErase(addr,param[0]);
    break;
  case cAMSW_FLASHDIR:
    PrintSummary(addr);
    break;
  case cAMSW_BOOT:
    if(nparam)Boot(addr,param[0]);
    else Boot(addr);
    break;
  case cAMSW_GETSTAT:
    PrintNodeStatus( addr);
    break; 
  case cAMSW_SSFRD:
    GetSSF( addr);
    break;
  case cAMSW_SSFWR:
    SetSSF( addr, param[0]);
    break;
  case cAMSW_BUSYLD:
    WriteBusyMask( addr, param[0], param[1]);
    break;
  case cAMSW_BUSYRD:
    ReadBusyMask( addr);
    break;
  case cAMSW_BUSYSTAT:
    ReadBusyStat( addr);
    break;
  case cAMSW_LV1:
    Command_Lv1(param[0]);
    break;
  case cAMSW_QLISTRD:
    ReadQList( addr);
    break;
 //  case cAMSW_FILECR:
//     CreateFlashFile(addr);
//     break;
  case cAMSW_COMMAND:
    for(int ii=0;ii<nparam;ii++)
      IOutput("0x%04x",param[ii]);
    IOutput("\n");
    Command(nparam,param);
    PrintRXDONE();
    PrintEvent();
    PrintBuildStat(GetBuildStat());
    break;

  case cAMSW_GCOMMAND:
    for(int ii=0;ii<nparam;ii++)
      IOutput("0x%04x",param[ii]);
    IOutput("\n");
    Command2(addr,param[0],nparam-1,&(param[1]));
    PrintRXDONE();
    PrintEvent();
    PrintBuildStat(GetBuildStat());
    break;

  default:
    IOutput("Command not implemented yet, use ff10\n");
    return -2;
  }

  return 0;
}
