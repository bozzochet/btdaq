//  class created by Giacomo Galilei jan-feb 2005
//
//  modified by phazz in may-june 2005 to use new rom monitor,
//  and to include it into Eduardo Cortina's TDR program.
//
//  sept 2005 phazz, trying to include Florian Hauler's work into this class, to add PCI
//
//   7.10.2005 phazz: modifying the class to be able to receive more than one fragment
//                  i.e. more than 8 KB of data in a single event.
//   9.10.2005 phazz: starting a big cleanup of the class
//
//  29.9.2006 phazz: pci can send more than one frag of data
//   1.10.2006 phazz: epp can send more than one frag of data
//   2.10.2006 phazz: write flash works
//   9.08.2007 pzuccon: general cleanup of include files and remove of some oddities
//  22.08.2007 pzuccon: Import from TestPCI: all the printout for the interactive use
//                                         the method to call a command from its hex code
//                                         update the output redirection method via a pointer to a function
//  19.01.2009 pzuccon: Added a Command2 function with an array as parameter instead of '...'
//   4.08.2009 mduranti: Including Lebedev libs to create a new communication method (kAMSW_JMDC) to communicate through JMDC (but also works directly on EPP)


#include<iostream>

#include <errno.h>
//#include <asm/bitops.h> // nedeed to comment this line to compile on fedora10
#include <sys/types.h> // needed to add this line to compile on fedora10
#include <sys/io.h> 
#include <sys/time.h>
#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include <sstream>
#include <math.h>

// Definition of AMSWcom class
#include "amswcomtest.h"

// C/C++ API for the amsw commands through EPP/PCI
#include "amsw_lib.h"

// include useful for CommandJMDC that uses C/C++ Zuccon/Lebedev API for the amsw commands through JMDC
#include "pzlib.h"

using namespace std;

CalibMemory Prog_3123, Prog_3ed3, Prog_3f83, Prog_3710_07;

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
  out=NULL; // no text output.
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

  Outfun=AMSWcom::OutputStd;
  TimeOut=5000; // time in ms
  DEBUG=0;
  memset(FlashSummary,0,sizeof(FlashSummary));
  memset(CmdArray,0,CMDSIZE*sizeof(CmdArray[0]));
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
    
  case kAMSW_JMDC:
    if (DEBUG) printf("Init... via JMDC\n");
    ret=InitJmdc();
    if (DEBUG) printf("ret=%d\n",ret);
    break;
  }
  
  return ret;

}


bool AMSWcom::InitJmdc ( )
{
  //printf("AMSWcom::InitJmdc -> Now this function is doing nothing!!!!!!!!!!!!!!!!!!!!!!!!!!\n");
  
  return SUCCESS;
}


// * Static methods to bring up and down the epp connection
bool AMSWcom::InitEpp ( )
{

// 	if( eppUp )
// 		return SUCCESS;
	
// 	if( ioperm( EPPBASE, 8, true ) )
// 		return ERROR;

// 	unsigned char stat_byte = 0x0, cont_byte = 0x0;

// 	// *  reset EPP
// 	outb(cont_byte, EPPCONTROL);
// 	usleep(1000);
// 	set_bit(C2, (unsigned long *)&cont_byte);
// 	outb(cont_byte, EPPCONTROL);

// 	// *  clear timeout bit
// 	stat_byte = inb(EPPSTATUS);
// 	if (test_bit(S0, (unsigned long *)&stat_byte))
// 		outb(stat_byte, EPPSTATUS);

// 	eppUp = true;

// 	return SUCCESS;


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
  case kAMSW_JMDC:
    ret=ShutJmdc();
    break;
  }

  return ret;
}


bool AMSWcom::ShutEpp ( )
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

bool AMSWcom::ShutJmdc() {
//  printf("AMSWcom::ShutJmdc -> Now this function is doing nothing!!!!!!!!!!!!!!!!!!!!!!!!!!\n");
  
  return SUCCESS;
}

void AMSWcom::SetInteractive() {
  sType=kINTER;
  if (hwtype==kAMSW_JMDC) Print_Level(kINTER);//function in pzlib
}

void AMSWcom::SetBatch() {
  sType=kBATCH;
  if (hwtype==kAMSW_JMDC) Print_Level(kBATCH);//function in pzlib
}


void AMSWcom::SetCalParMem(CalibMemory *calmem) {
  //IOutput("lowfactor=%lf  highfactor=%lf\n", calmem->SigLFactor,calmem->SigHFactor);
  memcpy(&CalParMem,calmem,sizeof(CalParMem));
  //IOutput("lowfactor=%lf  highfactor=%lf\n", CalParMem.SigLFactor,CalParMem.SigHFactor);
}


int AMSWcom::CommandJMDC(int args, unsigned short *params, int mode24) {
  // memset(Event,0,sizeof(Event));
  RX_DONE=send_AMSW_CMD(port, args, params, Event, &EventSize);//this function is the really function sending the command through JMDC and is defined in pzlib.h
  IOutput("CommandJMDC: RX_DONE=%d\n", RX_DONE);//only for debug
  if(RX_DONE!=0) IOutput("ERROR --> RX_DONE=%x\n",RX_DONE);
  //  IOutput("event before stripping:\n" );//only for debug
  //   IOutput("Event: "); for (int i=0; i<EventSize; i++) IOutput("%04x ", Event[i]); IOutput("\n");//only for debug
  

  IOutput("send_AMSW_CMD replies %d\n", RX_DONE);
  IOutput("event before stripping:\n" );
  for (int i=0; i<EventSize; i++) IOutput("%04x ", Event[i]); IOutput("\n");

  //IOutput("Qui c'è lo striping del comando inviato probabilmente ri-aggiunto alla risposta dalla JMDC, vedere se veramente c'è...\n");

  JMDC_CMD_SendStripping();
 //   IOutput("event after stripping:\n");//only for debug
//    IOutput("Event: "); for (int i=0; i<EventSize; i++) IOutput("%04x ", Event[i]); IOutput("\n");//only for debug

  if (EventSize>0) EventSize--;//to remove CRC from event
  
  if (EventSize>0) {
    BuildStat=Event[EventSize-1];
    CRC=Event[EventSize]; 
    //    if (DEBUG)
//    if (DEBUG) 
  IOutput("JMDC: crc= 0x%04x , DoFCS= 0x%04x\n", CRC, DoFCS());
    if (CRC!=DoFCS()) return (RX_DONE=65530); // return (RX_DONE=-6); changed because RX_DONE is unsigned short so -6 is trasformed to 65530
  }
  
  else { BuildStat=0; CRC=0; }
  
  
  if (mode24 && EventSize>0) { 
    EventSize=Convert24()+1;
    Event[EventSize-1]=BuildStat;
    Event[EventSize]=CRC;
  }

 return RX_DONE;
}


int AMSWcom::CommandJMDC(unsigned int addrl, unsigned char cmd, int args, ... ) {

  unsigned short addr=addrl & 0xffff;
  unsigned short addr2=addrl>>16;

  va_list ap;
  int cnt=0;

  if (args>(CMDSIZE-1)) { IOutput("too many args\n"); return -12345; }

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

  return CommandJMDC(cnt,CmdArray,(cmd==cAMSW_READPM));
}

void AMSWcom::JMDC_CMD_SendStripping() {

  unsigned short word;
  unsigned short wpref;
  unsigned short datatype;

  //IOutput("JMDC reply:\n");
  //for (int i=0; i<EventSize; i++) { IOutput("%04x ",Event[i]); if (i%8==7) IOutput("\n"); }
  //IOutput("\n");

  for (int cnt=0; cnt<5; cnt++) {
    word = Event[0];
    EventSize--;
    for (int ii=0; ii<EventSize; ii++) Event[ii]=Event[ii+1];
    wpref = word&0xff00;//select the first 8 bits ...
    datatype = word&0x001f;//select the last 5 bits...
    //    IOutput("stripping: word=%04x, cnt=%d, %04x %04x\n",word,cnt,wpref, datatype);//only for debug
    if (wpref==0x2e00) {//if the first 8 bits are "2e" this is the command 
      if (datatype!=0x1f) {//if all the last 5 bits are "up" (1) there's data type extension
	//IOutput("Stripping result case 1:\n");
	//IOutput("new evensize=%d\n", EventSize);
	//for (int i=0; i<EventSize; i++) { IOutput("%04x ",Event[i]); if (i%8==7) IOutput("\n"); }
	//IOutput("\n");
	return;
      }
      else {
	EventSize--;
	for (int ii=0; ii<EventSize; ii++) Event[ii]=Event[ii+1];
	//IOutput("Stripping result case 2:\n");
	//IOutput("new evensize=%d\n", EventSize);
	//for (int i=0; i<EventSize; i++) { IOutput("%04x ",Event[i]); if (i%8==7) IOutput("\n"); }
	//IOutput("\n");
	return;
      }
    }
  }

  //for (int ii=0; ii<EventSize; ii++) Event[ii]=Event[ii+1];
	//IOutput("Stripping result case 3:\n");
	//IOutput("new evensize=%d\n", EventSize);
	//for (int i=0; i<EventSize; i++) { IOutput("%04x ",Event[i]); if (i%8==7) IOutput("\n"); }
	//IOutput("\n");

  return;
}

void AMSWcom::Command(int args, unsigned short *params, int mode24) {
  switch (hwtype) {
  case kAMSW_EPP:
    CommandEPP(args,params,mode24);
    break;
  case kAMSW_PCI:
    CommandPCI(args,params,mode24);
    break;
  case kAMSW_JMDC:
    CommandJMDC(args,params,mode24);
    break;
  }


  ReplyStatus = Event[EventSize-1];
}


void AMSWcom::Command(unsigned int addrl, unsigned char cmd, int args, ... ) {

  unsigned short addr=addrl & 0xffff;
  unsigned short addr2=addrl>>16;

  int cnt=0;
  int mode24=0;

  if (args>(CMDSIZE-1)) { IOutput("too many args\n"); return; }

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

  if (DEBUG) { for (int i=0; i<cnt; i++) IOutput("%04x ", CmdArray[i]); IOutput("\n"); }

  Command(cnt, CmdArray,mode24); // if readpm, mode may be set to 24 bits
}

void AMSWcom::Command2(unsigned int addrl, unsigned char cmd, int args, ushort* params) {

  unsigned short addr=addrl & 0xffff;
  unsigned short addr2=addrl>>16;

  int cnt=0;
  int mode24=0;

    if (args>(CMDSIZE-1)) { IOutput("too many args\n"); return; }

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


  Command(cnt, CmdArray, mode24); // if readpm, mode may be set to 24 bits
}

void AMSWcom::RAWCommand(unsigned int addrl, int args, ushort* params) {

  unsigned short addr=addrl & 0xffff;
  unsigned short addr2=addrl>>16;

  int cnt=0;
  int mode24=0;

    if (args>(CMDSIZE-1)) { IOutput("too many args\n"); return; }

  if (addr!=0xffff) {
    CmdArray[cnt++]=addr; // addr can be 0xYY3f or 0x4000 or ... 
    if (addr2) CmdArray[cnt++]=addr2; // secondary address, useful if you use e.g. a Jinj.
  }

  for( int i = 0; i < args; i++ ) CmdArray[cnt++] = params[i];


  Command(cnt, CmdArray, mode24); // if readpm, mode may be set to 24 bits
}


void AMSWcom::CommandPCI(int args, unsigned short *params, int mode24) {
  //  unsigned char cmdch[2*];
  unsigned short val=0;



  int j=0; // command array counter

  for( int i = 0; i < args; i++ ) {
    val = params[i];
    //IOutput("0x%04x\n",params[i]);
    cmdch[j++]=val >> 8;
    cmdch[j++]=val & 0xff;
  }


  //for (int i=0; i<j; i+=2) IOutput("%02x%02x ", cmdch[i],cmdch[i+1]);
  //IOutput("\n");

  RX_DONE=SendCommandPCI(port, cmdch, j, Event, &EventSize, mode24);

  if (EventSize>0) { BuildStat=Event[EventSize-1]; CRC=Event[EventSize]; }
  else { BuildStat=0; CRC=0; }
 
}


void AMSWcom::CommandPCI (unsigned int addrl, unsigned char cmd, int args, ... ) {

  unsigned short addr=addrl & 0xffff;
  unsigned short addr2=addrl>>16;



  int cnt=0;

  if (args>(CMDSIZE-1)) { IOutput("too many args\n"); return; }

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
    IOutput("%s: Waiting for free link %i\n",__FUNCTION__,amsw_tx_ready(link));  
    tcounter++;
  }
  
  /* Send data out */	  

  // we have to take into account the possibility that we transmit more than one fragment of data...

  unsigned short bc=2; // bloc control: 2=begin of block, 
  unsigned short position=0;
  unsigned short limit=2*0xfff;
  while (csize>limit) { // more than one fragment
    ret = amsw_tx_packet(link, &command[position], limit, bc);
    //IOutput("csize=%x bc=%d  ret=%d\n",csize,bc,ret);
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
    //IOutput("tcounter=%d, fragsize=%d\n",tcounter, fragsize);

    rx_bc = 0;
    ret = amsw_rx_packet(link, &data[*datasize], fragsize, &rx_bc);

    *datasize=0;

    //IOutput("ret=%d, rx_bc=%d\n",ret,rx_bc);
  }
  
  bc|=1; //we add end of block
  
  ret = amsw_tx_packet(link, &command[position], csize, bc);
  //IOutput("csize=%x bc=%d  ret=%d\n",csize,bc,ret);

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
    //  IOutput("BC-Bits are: %d\n",rx_bc);
    
    *datasize+=fragsize;
    
    if ( ret < 0 ) return -4;  //RX failed ...

  /* Check data */
  if (rx_bc!=3 && (fragsize==0)) {
    //IOutput("rx_bc=%d\n",rx_bc);
    return -5; // simplified check, temporary
  }
      
  // now the block control is important, we need to get data until we get end of block...  
  fcounter++;
  } while ((rx_bc & 1)==0); // we are waiting for 'end of block'
  

  if (*datasize > 0) {
    CRC=(u_short(data[(*datasize)-2])<<8)+u_short(data[(*datasize)-1]);
    //IOutput("PCI: CRC=0x%04x   DoFCS= 0x%04x\n",CRC,DoFCS(data, (*datasize)-2));
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
    if (DEBUG) IOutput("%04x %04x %04x\n",Event[i],Event[i+1],Event[i+2]);
    val1=(Event[i]<<8) | (Event[i+1]>>8);
    val2=(( 0x00FF & Event[i+1])<<16) | Event[i+2];
    if (DEBUG) IOutput("%04x %04x\n", val1, val2);
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

void AMSWcom :: SetOutput( EppOutput * myOut )
{
	out = myOut;
}

void AMSWcom::SetOutput( OUTFUN aa){

  Outfun=aa;
}

void AMSWcom :: Output( string s )
{
	if( out )
	{
		out -> EppPrint( s );
	}
}

void AMSWcom :: OutputStd( std::string s )
        {

  std::cout<<s;
  return;
        }

void AMSWcom::OutputFile(std::string s){ 
  return;
}


void AMSWcom::FOutput (const char * fmt, ...){
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

void AMSWcom::IOutput (const char * fmt, ...){
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

int AMSWcom::GETRXDONE() {
  int error=0;
  if (GetHW()==kAMSW_EPP) { 
    if (GetRX_DONE()!=0x4180) error=1; 
  }
  else if (GetHW()==kAMSW_PCI) {
    if (GetRX_DONE()!=0) error=1; 
  }
  else if (GetHW()==kAMSW_JMDC) {
    if (GetRX_DONE()!=0) error=1; 
  }
  
  return error;
}

int AMSWcom::PrintRXDONE(char* message) {
  int error=0;

  if (GetHW()==kAMSW_EPP) { 
    if (message) sprintf(message,"EPP rxdone = %04x\n",GetRX_DONE());
    else IOutput("EPP rxdone = %04x\n",GetRX_DONE());
    if (GetRX_DONE()!=0x4180) error=1; 
  }
  else if (GetHW()==kAMSW_PCI) {
    if (message) sprintf(message,"PCI rxdone = %d\n",(short)GetRX_DONE());
    else IOutput("PCI rxdone = %d\n",(short)GetRX_DONE());
    if (GetRX_DONE()!=0) error=1; 
  }
  else if (GetHW()==kAMSW_JMDC) {
    if (message) sprintf(message,"JMDC rxdone = %d\n",(short)GetRX_DONE());
    else IOutput("JMDC rxdone = %d\n",(short)GetRX_DONE());
    if (GetRX_DONE()!=0) error=1; 
  }

  if (GetEventSize()) {
    IOutput("eventsize = %d\n",GetEventSize()); 
    IOutput("reply status = 0x%04x\n",GetReplyStatus());
    if ((GetReplyStatus() & 0x20) == 0) IOutput("\033[0;7mData have a group structure\033[0m\n");
    if (GetReplyStatus() & 0x200) IOutput("\033[1;31mBuild errors\033[0m\n");
    if (GetReplyStatus() & 0x400) IOutput("\033[1;31mFront-end power problems\033[0m\n");
    IOutput("\n");
  }
  
  return error;
}

void AMSWcom::HKRead(unsigned int addr) {
  IOutput("Asking for housekeeping info to address 0x%08x\n",addr);

  Command(addr, cAMSW_HKINFO, 0);

  PrintRXDONE();

  int cnt=0;
  IOutput("Program version ID: 0x%04x\n",Event[cnt++]);
  IOutput("SubD    version ID: 0x%04x\n",Event[cnt++]);
  IOutput("Node status:        0x%04x\n",Event[cnt++]);
  IOutput("Last event number:  %d\n",Event[cnt++]);
  IOutput("Avg process time:   %d us\n",Event[cnt++]*20);
  IOutput("Calibration type:   0x%04x\n",Event[cnt++]);
  IOutput("Calibration status: 0x%04x\n",Event[cnt++]);
  IOutput("Mean clust. len. S: %d\n",Event[cnt++]);
  IOutput("Mean clust. len. K: %d\n",Event[cnt++]);
  IOutput("Num. cluster/evt S: %5.2f\n",Event[cnt++]/1024.);
  IOutput("Num. cluster/evt K: %5.2f\n",Event[cnt++]/1024.);
  IOutput("Power failure S:    %d\n",Event[cnt++]);
  IOutput("Power failure K:    %d\n",Event[cnt++]);
  unsigned short redmod=Event[cnt++];
  IOutput("\nRed mode: Dyn.ped: %s   CN output: %s     Laser mode: %s\n", (redmod&1)?"on":"off", (redmod&2)?"on":"off", (redmod&4)?"on":"off");
  IOutput("Red mode: clus. size lim.: %s,    clust. num. lim. S: %s,    clust. num. lim. K:%s,   single chan. small clust. exl.: %s",(redmod&8)?"on":"off",(redmod&16)?"on":"off",(redmod&32)?"on":"off",(redmod&64)?"on":"off");
  unsigned short calibtables=Event[cnt++];
  IOutput("\n");
  if (calibtables) {
    if (calibtables & 1) IOutput("No calibration data loaded\n");
    if (calibtables & 0x100) IOutput("Pedestal data modified since last calibration\n");
    if (calibtables & 0x200) IOutput("Raw sigma data modified since last calibration\n");
    if (calibtables & 0x400) IOutput("Low sigma data modified since last calibration\n");
    if (calibtables & 0x800) IOutput("High sigma data modified since last calibration\n");
    if (calibtables & 0x1000) IOutput("Flag data modified since last calibration\n");
    if (calibtables & 0x2000) IOutput("Sigma data modified since last calibration\n");
  }
  else IOutput("Calibration tables are ok\n");

}

void AMSWcom::SetSSF(unsigned int addr, unsigned short setting) {
  IOutput("Setting SSF mode to %x for address 0x%08x\n",setting,addr);

  Command(addr, cAMSW_SSFWR, 1, setting);
  PrintRXDONE();
  if (GetEventSize()) ShowGroupReplies();
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
  if (GetEventSize()) ShowGroupReplies();
}

void AMSWcom::Boot(unsigned int addr, unsigned short fname) {
  if (DEBUG) IOutput("welcome to boot2...\n");

  IOutput("Booting address 0x%08x \n",addr);
  if(fname!=0){
    IOutput(" with program 0x%04x\n", fname);
    Command(addr, cAMSW_BOOT, 1, fname );
  }
  else Command(addr, cAMSW_BOOT, 0);

  PrintRXDONE();
  if (GetEventSize()) ShowGroupReplies();
}

void AMSWcom::ReadFlashFile(unsigned int addr, unsigned short file) {

  int ret=0;

  IOutput("Saving file 0x%04x of address 0x%08x in file.dat\n",file,addr);

  FlashRead(addr,file);
  PrintRXDONE();
  ret=SaveFlashFile("file.dat");

  IOutput((ret)?"ERROR\n":"SUCCESS\n");
}

void AMSWcom::FlashLoad(unsigned int addr, unsigned short fname) {
  IOutput("Address 0x%08x will load program id 0x%04x\n",addr,fname);
  Command(addr, cAMSW_FLASHLD, 1, fname );
  PrintRXDONE();
  if (GetEventSize()) ShowGroupReplies();
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
  if (GetEventSize()) ShowGroupReplies();
}

void AMSWcom::FlashDefault(unsigned int addr, unsigned short fname, unsigned short def) {
  IOutput("Address 0x%08x will set to mode %d program id 0x%04x\n",addr,def,fname);
  Command(addr, cAMSW_FLASHDF, 2, fname, def );
  PrintRXDONE();
  if (GetEventSize()) ShowGroupReplies();
}

void AMSWcom::WriteDelay(unsigned int addr, unsigned short delay) {
  IOutput("Address 0x%08x will have a delay of 0x%04x (%2.2f us)\n",addr,delay,(float)delay*0.02);
  Command(addr,cAMSW_DELAYWR,1,delay);
  PrintRXDONE();
  if (GetEventSize()) ShowGroupReplies();
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
  SDprocRead(addr,0);

  PrintRXDONE();

  for (int i=0; i<1024; i++) daccal[i]=Event[i];

  char fname[100];
  sprintf(fname,"out%d.dac",dac);
  FILE *f=fopen(fname,"wt");
  for (int i=0; i<1024; i++) fprintf(f,"%d\n",daccal[i]);
  fclose(f);

}

void AMSWcom::SDProcRead(unsigned int addr, unsigned short par) {
  
  SDprocRead(addr,par);
  PrintRXDONE();

  if (par==1) {
    IOutput("Pedestals S: mean = %d, sig = %d\n", Event[0], Event[1]);
    IOutput("Pedestals K: mean = %d, sig = %d\n", Event[2], Event[3]);
    IOutput("Sigma S    : mean = %6.3f, sig = %6.3f\n", Event[4]/8.0, Event[5]/8.0);
    IOutput("Sigma K    : mean = %6.3f, sig = %6.3f\n", Event[6]/8.0, Event[7]/8.0);
  } 
  else if (par==0x1001) {
    // jint-t read status command
    unsigned short ndisc=Event[4];
    if (ndisc) {
      IOutput("We have %d discrepancies\n\n", ndisc);
      for (int i=5; i<GetEventSize()-1; i+=2) {
	//      IOutput("Parameter: 0x%04x    Value: 0x%04x\n", Event[i], Event[i+1]);
	TranslateState(9, Event[i], Event[i+1]); // crate 9: does not exist
      }
    }
  }
  else PrintEvent();
}

void AMSWcom::SDProc(unsigned int addr, unsigned short cmd) {
  Command(addr,cAMSW_CALDAC,1,cmd);

  PrintRXDONE();
  if (GetEventSize()) ShowGroupReplies();
}

void AMSWcom::SDProc(unsigned int addr, unsigned short cmd, unsigned short par) {
  Command(addr,cAMSW_CALDAC,2,cmd, par);

  PrintRXDONE();
  if (GetEventSize()) ShowGroupReplies();
}


void AMSWcom::SDProc(unsigned int addr, unsigned short cmd, unsigned short par, unsigned short par2) {
  Command(addr,cAMSW_CALDAC,3,cmd, par, par2);

  PrintRXDONE();
  if (GetEventSize()) ShowGroupReplies();
}

void AMSWcom::Calibrate(unsigned int addr) {
 
  Command(addr,cAMSW_CALPED,0);

}

void AMSWcom::Calibrate(unsigned int addr, unsigned short par) {
  IOutput("Calibrating address 0x%08x with mode %d\n",addr,par);
  Command(addr,cAMSW_CALPED,1,par);
  PrintRXDONE();
  if (GetEventSize()) ShowGroupReplies();
}

void AMSWcom::Calibrate(unsigned int addr, unsigned short par, unsigned short par2) {
  IOutput("Calibrating address 0x%08x with mode %d ans parameter %%04x\n",addr,par,par2);
  Command(addr,cAMSW_CALPED,2,par,par2);
  PrintRXDONE();
  if (GetEventSize()) ShowGroupReplies();
}

void AMSWcom::SetParameter(unsigned int addr, unsigned short parnam, unsigned short parcont) {

  IOutput("\nSetting parameter 0x%04x to value 0x%04x on address 0x%04x\n\n", parnam, parcont, addr);

  SetParameter(addr,0x1000, parnam , parcont);
  PrintRXDONE(); 
}

void AMSWcom::SetParameter(unsigned int addr, unsigned short type, unsigned short name, unsigned short val) {

  type|=1;

  Command(addr, cAMSW_SDCONFIG, 3, type, name, val);
}

void AMSWcom::SetParameter(unsigned int addr, unsigned short type, unsigned short name1, unsigned short val1,  unsigned short name2, unsigned short val2) {

  type|=2;

  Command(addr, cAMSW_SDCONFIG, 5, type, name1, val1, name2, val2);
}

void AMSWcom::GetParameter(unsigned int addr, unsigned short parnam) {

  unsigned short parval=GetParameter(addr,0x1000, parnam);

  IOutput("\nParameter 0x%04x of address 0x%04x has a value of 0x%04x\n\n", parnam, addr, parval);
  
  PrintRXDONE();
}

unsigned short AMSWcom::GetParameter(unsigned int addr, unsigned short type, unsigned short name) {
  type|=1;

  Command(addr, cAMSW_RDCONFIG, 2, type, name);

  return Event[1];
}


void AMSWcom::GetCalibration(unsigned int addr, FILE *textfile) {
  //CHECK_FOR_EPPUP;
  unsigned short iPed[1024];
  unsigned short iSig[1024];
  unsigned short iSigH[1024];
  unsigned short iSigr[1024];
  unsigned short iSta[1024];
  
  // * read_pm( address, length, *data )
  ReadMemory(addr,CalParMem.Ped,0x400,CalParMem.MemPed,1);
  memcpy(iPed,Event,sizeof(iPed));

  ReadMemory(addr,CalParMem.SigL,0x400,CalParMem.MemSigL,1);
  memcpy(iSig,Event,sizeof(iSig));

  ReadMemory(addr,CalParMem.SigH,0x400,CalParMem.MemSigH,1);
  memcpy(iSigH,Event,sizeof(iSigH));

  ReadMemory(addr,CalParMem.Stat,0x400,CalParMem.MemStat,1);
  memcpy(iSta,Event,sizeof(iSta));

  ReadMemory(addr,CalParMem.Sigr,0x400,CalParMem.MemSigr,1);
  memcpy(iSigr,Event,sizeof(iSigr));

  if (textfile) { 
    fprintf(textfile,"Pedestals: %s memory, start=0x%04x\n",(CalParMem.MemPed==kDSP_PM)?"PM":"DM",CalParMem.Ped);
    for (int i=0; i<0x400; i++) { fprintf(textfile,"0x%04x  ",iPed[i]); if (!((i+1)%16)) fprintf(textfile,"\n");}

    fprintf(textfile,"\n\nSigma Low: %s memory, start=0x%04x\n",(CalParMem.MemSigL==kDSP_PM)?"PM":"DM",CalParMem.SigL);
    for (int i=0; i<0x400; i++) { fprintf(textfile,"0x%04x  ",iSig[i]); if (!((i+1)%16)) fprintf(textfile,"\n");}

    fprintf(textfile,"\n\nSigma High: %s memory, start=0x%04x\n",(CalParMem.MemSigH==kDSP_PM)?"PM":"DM",CalParMem.SigH);
    for (int i=0; i<0x400; i++) { fprintf(textfile,"0x%04x  ",iSigH[i]); if (!((i+1)%16)) fprintf(textfile,"\n");}

    fprintf(textfile,"\n\nSigma Raw: %s memory, start=0x%04x\n",(CalParMem.MemSigr==kDSP_PM)?"PM":"DM",CalParMem.Sigr);
    for (int i=0; i<0x400; i++) { fprintf(textfile,"0x%04x  ",iSigr[i]); if (!((i+1)%16)) fprintf(textfile,"\n");}

    fprintf(textfile,"\n\nStatus: %s memory, start=0x%04x\n",(CalParMem.MemStat==kDSP_PM)?"PM":"DM",CalParMem.Stat);
    for (int i=0; i<0x400; i++) { fprintf(textfile,"0x%04x  ",iSta[i]); if (!((i+1)%16)) fprintf(textfile,"\n");}
  }

  //  IOutput("siglfactor=%f\n",  CalParMem.SigLFactor);

  for(int i=0; i<1024; i++) {
      ped[i]=(double)(iPed[i])/CalParMem.PedFactor;
      sig[i]=(double)(iSig[i])/CalParMem.SigLFactor;
      sigr[i]=(double)(iSigr[i])/CalParMem.SigrFactor;
      sta[i]=(iSta[i]);
  }
}

ushort AMSWcom::GetCalibration(unsigned int addr,ushort par) {

  IOutput("Getting calibration from address 0x%08x with par %d\n",addr,par);
  Command(addr,cAMSW_CALRED,1,par);
  PrintRXDONE();

  PrintEvent();
  return EventSize;
//   //CHECK_FOR_EPPUP;

    
//   unsigned short iPed[1024];
//   unsigned short iSig[1024];
//   unsigned short iSigr[1024];
//   unsigned short iSta[1024];
  
//   // * read_pm( address, length, *data )
//   ReadMemory(addr,CalParMem.Ped,0x400,CalParMem.MemPed,1);
//   memcpy(iPed,Event,sizeof(iPed));

//   ReadMemory(addr,CalParMem.SigL,0x400,CalParMem.MemSigL,1);
//   memcpy(iSig,Event,sizeof(iSig));

//   ReadMemory(addr,CalParMem.Stat,0x400,CalParMem.MemStat,1);
//   memcpy(iSta,Event,sizeof(iSta));

//   ReadMemory(addr,CalParMem.Sigr,0x400,CalParMem.MemSigr,1);
//   memcpy(iSigr,Event,sizeof(iSigr));

//   //  IOutput("siglfactor=%f\n",  CalParMem.SigLFactor);

//   for(int i=0; i<1024; i++) {
//       ped[i]=(double)(iPed[i])/CalParMem.PedFactor;
//       sig[i]=(double)(iSig[i])/CalParMem.SigLFactor;
//       sigr[i]=(double)(iSigr[i])/CalParMem.SigrFactor;
//       sta[i]=(iSta[i]);
//   }
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
    //IOutput("id=%d, mask=0x%x\n",id,mask.ID[id]);
  }

  //IOutput("there are %d masks\n", mask.Nmask);
  PrintRXDONE();

  if (EventSize && sType==kINTER) ShowConnect(mask);  
  return mask;
}


void AMSWcom::WriteSlaveMask(unsigned int addr, unsigned short id, unsigned int mask) {
  unsigned short val1,val2;

  mask&=0xffffff;
  id&=0xff;
  val1=(mask>>16)+(id<<8);
  val2=mask&0xffff;

  //  IOutput("mask= 0x%04x 0x%04x\n", val1,val2);//only for debug 

  WriteSlaveMask(addr,val1,val2);
}

void AMSWcom::WriteSlaveMask(unsigned int addr, unsigned short val1, unsigned short val2) {
  IOutput("Writing slave mask for address 0x%08x\n",addr);
  Command(addr,cAMSW_MASKLD,2,val1,val2);
  PrintRXDONE();
  if (GetEventSize()) ShowGroupReplies();
}

void AMSWcom::WriteBusyMask(unsigned int addr, unsigned int mask) {

  unsigned short val1,val2;

  mask&=0xffffff;
  val1=(mask>>16);
  val2=mask&0xffff;

  //IOutput("mask= 0x%04x 0x%04x\n", val1,val2); 

  WriteBusyMask(addr,val1,val2);
}

void AMSWcom::WriteBusyMask(unsigned int addr, unsigned short val1, unsigned short val2) {
  IOutput("Setting busy mask %04x %04x to address %08x\n", val1,val2, addr);
  Command(addr,cAMSW_BUSYLD,2,val1,val2);
  PrintRXDONE();
  if (GetEventSize()) ShowGroupReplies();
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
  if (GetEventSize()) ShowGroupReplies();
}

void AMSWcom::SDprocRead(unsigned int addr, unsigned short mode) {
  Command(addr,cAMSW_SDPROC_RD,1,mode);
}

void AMSWcom::SDprocRead(unsigned int addr, unsigned short mode, unsigned short par) {
  Command(addr,cAMSW_SDPROC_RD,2,mode,par);
}

void AMSWcom::CalibRead(unsigned int addr, unsigned short mode) {
  Command(addr,cAMSW_CALPEDRD,1,mode);
}

unsigned short AMSWcom::ReadMode(unsigned int addr) {

  Command(addr,cAMSW_RDMODE,0);
  
  unsigned short mode=Event[0];
  unsigned short status=0, saddr=0;

  //  IOutput("Address 0x%08x is in the%s%s%s processing mode\n",addr,(!mode)?" NO":"",(mode&1)?" RAW":"",(mode&2)?" RED":"");
  //  PrintRXDONE();
  
  if ((GetReplyStatus() & 0x20) == 0 ) { // we have a group structure
    int cnt=0;
    for (int i=0; i<GetEventSize(); i++) {
      ushort len=Event[cnt++];
      if (len==2) {
	mode = Event[cnt++];
	status = Event[cnt++];
	saddr = status & 0x1f;
	IOutput("Address 0x%02x3f is in the%s%s%s processing mode\n",saddr,(!mode)?" NO":"",(mode&1)?" RAW":"",(mode&2)?" RED":"");
      }
      else if (len==1) 
	{
	  char msg[50];
	  sprintf(msg," ");
	  status = Event[cnt++];
	  saddr = status & 0x1f;	  
	  IOutput("Address 0x%02x3f has status 0x%04x", saddr, status);
	  if (status == 0x1000) sprintf(msg,"\033[0;31mABORT\033[0m");
	  if (status == 0x1800) sprintf(msg,"\033[0;31mERROR\033[0m");
	  if (status == 0x2800) sprintf(msg,"\033[0;31mTIMEOUT\033[0m");
	  IOutput(" %s\n",msg);
	}
    }
  }
  else {
    IOutput("Address 0x%08x is in the%s%s%s processing mode\n",addr,(!mode)?" NO":"",(mode&1)?" RAW":"",(mode&2)?" RED":"");
  }
  PrintRXDONE();
  
  return mode;
}

void AMSWcom::EventReset(unsigned int addr) {
  IOutput("Clearing Event FIFO and last event number of address 0x%08x\n",addr);
  Command(addr,cAMSW_EVNTRST,0);
  PrintRXDONE();
  if (GetEventSize()) ShowGroupReplies();
}

unsigned short AMSWcom::GetLastEventN(unsigned int addr) {
  IOutput("Getting last event number from address 0x%08x\n",addr);

  Command(addr,cAMSW_EVNTNUM,0);
  PrintRXDONE();
  PrintEvent();
 
  IOutput("Last event number of address 0x%08x is 0x%04x\n",addr,Event[0]);

  return Event[0];
}

unsigned short AMSWcom::GetEvent(unsigned int addr) {
  IOutput("Getting event from address 0x%08x\n", addr);
  Command(addr,cAMSW_EVNTGET,0);
  PrintRXDONE();
  PrintEvent();
  return EventSize;
}

void AMSWcom::GetEvent(unsigned int addr, int trigger) {
  IOutput("Getting event from address 0x%08x\n", addr);
  if (trigger) { Command_Lv1(); usleep(100000); }
  GetEvent(addr);
  PrintEvent();
  InterpretData();
  PrintRXDONE();
}

void AMSWcom::GetEvent(unsigned int addr, int trigger, int limit) {
  IOutput("Looping GetEvent from address 0x%08x, until event size is > %d\n", addr,limit);
  unsigned short size=0;
  int cnt=0;
  do {
    if (trigger) { Command_Lv1(); usleep(10000); }
    GetEvent(addr);
    if (GetRX_DONE()!=((GetHW()==kAMSW_EPP)?0x4180:0)) break;
    size=GetEventSize();
    if (cnt++%1000==0) { 
      SDProc(addr,7); // crc check of the calib data
      HKRead(addr);
      //ReadDM(addr,0x3de0,16);
      //PrintEvent();
      //ReadDM(addr,0x3dd0,16);
      //PrintEvent();
    }
  } while(size<limit);
  PrintEvent();
  PrintRXDONE();
}

void AMSWcom::PrintEvent() {
  int length=EventSize;
  for (int i=0; i<length; i++) {
    if (i%8==0) IOutput("%5d: ",i);
    IOutput("%04x ",Event[i]);
    if ((i+1)%8==0) IOutput("\n");
  }
  //  IOutput("CRC= 0x%04x\n",GetCRC());  
  IOutput("\n");
}

void AMSWcom::PrintEvent(int length) {
  FILE *f=fopen("dump.txt","wt");
  if (length==-1) length=GetEventSize();
  for (int i=0; i<length; i++) {
    if (i%16==0) IOutput("%5d: ",i);
    IOutput("%04x ",Event[i]);
    if ((i+1)%16==0) IOutput("\n");
    if (f) fprintf(f,"%04x\n", Event[i]);
  }
  //  IOutput("CRC= 0x%04x\n",GetCRC());  
  IOutput("\n");
  IOutput("Event size = %d\n", GetEventSize());
  if (f) fclose(f);
}

int AMSWcom::CommandEPP(int args, unsigned short *params, int mode24) {
  int tt;
  if( (tt=Check_for_EPPUP()) !=0 ) return tt;

  int  i;
  unsigned short ret;
  unsigned short offset=1;
  
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

    //    IOutput("nw=0x%04x length=0x%04x txst=%x rxdone=0x%04x\n",nw,length,txst,RX_DONE);
    //if ((RX_DONE & 0xE000) == 0x4000 ) if ((RX_DONE & 0x180) != 0) return RX_DONE;
  }

  txst|=0x2000;
  tx_start ( cBM_PORT_TXBUF(port)+length, txst+nw );
  
  EventSize=0;
  unsigned short len=0;
  do {
    RX_DONE=rx_done();
    if ( (RX_DONE & 0xE000) == 0x4000 ) {
      //IOutput("rx status=%04x\n", EPP_RX_DONE);      
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
      //IOutput("COMMAND: RX errors, status %04x", RX_DONE);
    }
  } while((len>0)&&((RX_DONE & cPORT_RXBIT_BC2)==0)); // we are waiting for 'end of block'

  if (EventSize==0) return 0;

  EventSize--;

  if (EventSize>0) {
    BuildStat=Event[EventSize-1];
    CRC=Event[EventSize]; 
    if (DEBUG) IOutput("EPP: crc= 0x%04x , DoFCS= 0x%04x\n", CRC, DoFCS());
    if (CRC!=DoFCS()) return (RX_DONE=65530); // return (RX_DONE=-6); changed because RX_DONE is unsigned short so -6 is trasformed to 65530
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

  if (args>(CMDSIZE-1)) { IOutput("too many args\n"); return -12345; }

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

void AMSWcom::Command_Lv1(int ntrig) {
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
  //IOutput( "\n Busy flag before sending LV1 - %02x \n", flag);

  lv1_start();

  flag=busy_stat();
  //IOutput( " Busy flag right after LV1 - %02x \n", flag);

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
  command[cnt++]=length+1; // fcs must be included into the data block, good luck!
  command[cnt++]=start;

  unsigned short startfcs=cnt;
 
  // data are stored the Event array
  //IOutput("before loop\n");
  for (int i=0; i<length*3/2; i++) command[cnt++]=Event[i];
  //IOutput("after loop\n");
  //for (int i=0; i<cnt; i++) IOutput("i=%4d  command=0x%04x\n",i, command[i]);

  unsigned short fcs=DoFCS(&command[startfcs],cnt-startfcs);
  command[cnt++]=fcs;


  Command(cnt,command,0);
  //IOutput("after command\n");

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

  command[cnt++]=0x2e00+cAMSW_WRITEDM;
  command[cnt++]=0x4000 | (length+1); // the fcs must be included into the data block
  command[cnt++]=start;

  unsigned short startfcs=cnt;
  // data are stored the Event array

  for (int i=0; i<length; i++) command[cnt++]=Event[i];
  
  unsigned short fcs=DoFCS(&command[startfcs],cnt-startfcs);
  command[cnt++]=fcs;

  Command(cnt,command,0);

  // some crc checks should be added there...
}

void AMSWcom::WriteDM(unsigned int amswaddr, char *datafil) {//speriamo non sia ambigua con quella sopra...

  IOutput("writing into Data Memory from file %s\n",datafil);

  FILE *infil=fopen(datafil,"rt");
  if (!infil) { IOutput("file %s has a problem\n", datafil); return; }

  unsigned short addr=0,len=0;
  fscanf(infil,"%*s %hx",&addr);
  fscanf(infil,"%*s %hx",&len);

  IOutput("start=0x%04x, len=0x%04x\n",addr,len);

  unsigned short val=0;
  int cnt=0;  

  for (int i=0; i<len; i++) {
    if (!(i%16)) fscanf(infil,"%*s");
    fscanf(infil,"%hx",&val);
    //IOutput("%4d: %04x\n",i,val);
    Event[cnt++]=0xffff & val;
  } 
  
  WriteMemory(amswaddr,addr,cnt,kDSP_DM);
  PrintRXDONE();
}

void AMSWcom::WritePMCalib(unsigned int amswaddr, unsigned short addr, void *data, unsigned short size, int mode) {

  IOutput("writing into Program Memory\n");

  int cnt=0, val=0;
  
  if (mode==0) {
    double *array=(double*)data;
    
    for (int i=0; i<size; i+=2) {
      //IOutput("array[%d]=%4.2lf *8=%5.2lf  ",i,array[i],8*array[i]);
      val=(int)(round(array[i])); // val is supposed to be a 24-bits number
      val&=0xffffff; // now it is really a 24-bits value...
      val=val<<8;
      //IOutput("val=%d, %08x ",val,val);
      //IOutput("0x%06x  ", val);
      Event[cnt++]=val>>8;
      Event[cnt]=(0xff & val)<<8;
      val=(int)(array[i+1]);
      val=val<<8;
      val&=0xffffff;
      //IOutput("0x%06x  ", val);
      Event[cnt++]+=val>>16;
      Event[cnt++]=0xffff & val;
      //IOutput("   0x%04x  0x%04x  0x%04x\n",Event[cnt-3],Event[cnt-2],Event[cnt-1]);
    } 
  }
  else {
    int *array=(int*)data;
    
    for (int i=0; i<size; i+=2) {
      val=array[i];
      val=val<<8;
      //IOutput("0x%06x  ", val);
      Event[cnt++]=val>>8;
      Event[cnt]=(0xff & val)<<8;
      val=array[i+1];      
      val=val<<8;
      //IOutput("0x%06x  ", val);
      Event[cnt++]+=val>>16;
      Event[cnt++]=0xffff & val;
      //IOutput("   0x%04x  0x%04x  0x%04x\n",Event[cnt-3],Event[cnt-2],Event[cnt-1]);
    }
  }

  //  int rsize=0;
  //  int reply=SendCommand(portid, tdrcommand,cnt,dd,&rsize);
  //  IOutput("WritePMCalib, cnt=%d, sizeof(cmd)=%d, reply=%d\n",cnt,sizeof(tdrcommand),reply);
  WriteMemory(amswaddr,addr,size,kDSP_PM);
  PrintRXDONE();
}

void AMSWcom::WriteDMCalib(unsigned int amswaddr, unsigned short addr, void *data, unsigned short size) {

  IOutput("writing into Data Memory\n");
  double *array=(double*)data;
  int cnt=0,val=0;
  
  for (int i=0; i<size; i++) {
    //IOutput("array[%d]=%4.2lf *8=%5.2lf  ",i,array[i],8*array[i]);
    val=(int)(round(array[i]));
    //    val=val<<8;
    //IOutput("val=%d, %08x \n",val,val);

    Event[cnt++]=0xffff & val;
  } 
  

  WriteMemory(amswaddr,addr,cnt,kDSP_DM);
  PrintRXDONE();
}

void AMSWcom::WriteCalibParam(unsigned int amswaddr, char *filename) {

  FILE *f=0;
  f=fopen(filename,"rt");

  if (f==0) return ;

  char chain[MAX_STRN];

  int cnt=-1;
  unsigned short val=0, addr=0;
  while (1) {
    fgets(chain, MAX_STRN, f);
    if (feof(f)) break;
    //IOutput("line %2d: %s\n",cnt,chain);
    sscanf(chain,"%hx", &val);
    //IOutput("val = %04x\n",val);
    if (cnt==-1) addr=val; 
    else Event[cnt]=0xffff & val;
    cnt++;
  }

  unsigned short fcs=DoFCS(&Event[0],cnt);
  IOutput("fcs=%x\n",fcs);

  WriteMemory(amswaddr,addr,cnt,kDSP_DM);
  PrintRXDONE();
}

void AMSWcom::PerformIO(unsigned int addrl, unsigned short seglen, unsigned short segaddr) {

  unsigned short addr=addrl & 0xffff;
  unsigned short addr2=addrl>>16;

  unsigned short command[EVENTSIZE+5];
  unsigned short val;

  unsigned int cnt=0;

  if (addr!=0xffff) {
    command[cnt++]=addr; // addr can be 0xYY3f or 0x4000 or ...
    if (addr2) command[cnt++]=addr2; // secondary address, useful if you use e.g. a Jinj.
  }

  command[cnt++]=0x2e00+cAMSW_IO;
  command[cnt++]=seglen;
  command[cnt++]=segaddr;

  unsigned short startfcs=cnt;
  // data are stored the Event array

  unsigned short length=seglen & 0x3fff; // length includes the fcs

  for (int i=0; i<length-1; i++) command[cnt++]=Event[i];
  
  unsigned short fcs=DoFCS(&command[startfcs],cnt-startfcs);
  command[cnt++]=fcs;

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

  IOutput("FlashWrite: length=%d\n",length);

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
  //IOutput("fcs=%x\n",fcs);
  //fcs=DoFCS(&command[start],cnt-start);
  //IOutput("fcs=%x\n",fcs);

  //for (int i=0; i<cnt; i++) IOutput("%04x\n",command[i]);

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
  
  //IOutput("fcs=%x\n",fcs);

  for (int i=0; i<cnt; i++) IOutput("%04x ", command[i]);  

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
  if(sType==kINTER) qevent->Print();
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
  //if (lerror) IOutput("lecroy: error=%x\n",lerror);
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
  if (DEBUG) IOutput("writing qlist...\n");
  unsigned short addr=addrl & 0xffff;
  unsigned short addr2=addrl>>16;

  unsigned short command[EVENTSIZE+5];
 
  unsigned int cnt=0;

  if (addr!=0xffff) {
    command[cnt++]=addr; // addr can be 0xYY3f or 0x4000 or ...
    if (addr2) command[cnt++]=addr2; // secondary address, useful if you use e.g. a Jinj.
  }

  command[cnt++]=0x2e00+cAMSW_LRSRD;
  unsigned short sizeloc=cnt;

  unsigned short start=cnt; // starting from next command, we will need to compute a crc
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

  if (DEBUG)  { for (int i=0; i<cnt; i++) IOutput("%04x ", command[i]); IOutput("\n"); }

  Command(cnt,command,0);
}

unsigned short AMSWcom::rx_status (){
  outb( ioPORT_RXST(port), EPPADDRESS );
  return inw(EPPDATA);
}

unsigned short AMSWcom::rx_done(){
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
  //IOutput("Elapsed time: %ld ms\n", (tv2.tv_sec-tv1.tv_sec)*1000+ (tv2.tv_usec-tv1.tv_usec)/1000);

  return m;
}

unsigned short AMSWcom::rx_len(){
  outb( ioPORT_RXAD(port), EPPADDRESS );
  return (inw( EPPDATA ) & 0xFFF);
}

void AMSWcom::tx_start(unsigned short addr, unsigned short stat){
  outb(ioPORT_TXAD(port),  EPPADDRESS);
  outw(addr, EPPDATA);
  
  outb( ioPORT_TXST(port),  EPPADDRESS);
  outw(stat, EPPDATA);
}


void AMSWcom::bmrd_init(){
  outb( ioBM_ADDR_RD,	   EPPADDRESS );
  outw( cBM_PORT_RXBUF(port),  EPPDATA );
  outb( ioBM_DATA_RD,	   EPPADDRESS );
}

void AMSWcom::bmwr_init(){
  outb( ioBM_ADDR_WR,	  EPPADDRESS );
  outw( cBM_PORT_TXBUF(port), EPPDATA );
  outb( ioBM_DATA_WR,	  EPPADDRESS );
}

void AMSWcom::crcrd_init(){
  outb(ioBM_CRC_RD, EPPADDRESS);
  outw(0x0000, EPPDATA);
}

unsigned short AMSWcom::crcrd_end(){
  unsigned short crc;
  outb( ioBM_CRC_RD, EPPADDRESS);
  crc = inw(EPPDATA);
  if( crc != 0 )
    FOutput( COL_STRONG "Error reading CRCrd " COL_EVIDENT "0x%04x != 0\n", crc );
  return crc;
}

void AMSWcom::crcwr_init(){
  outb(ioBM_CRC_WR, EPPADDRESS);
  outw(0x0000, EPPDATA);
}

unsigned short AMSWcom::crcwr_end(){
  unsigned short crc;
  outb(ioBM_CRC_WR, EPPADDRESS);
  crc = inw(EPPDATA);
  //	if( crc != 0 )
  //		FOutput( COL_STRONG "Error reading CRCwr " COL_EVIDENT "0x%04x != 0\n", crc );
  return crc;
}

unsigned short AMSWcom::DoFCS(unsigned char *data, int size){
  // from Kounine + Koutsenko documentation, p30
  
  unsigned short fcs=0xffff;
  
  for(int i=0; i<size; i++) fcs=FCS_Table[(fcs>>8)^data[i]]^(fcs<<8);
  
  //IOutput("DoFCS = %04x\n",fcs);
  
  return fcs;
}

unsigned short AMSWcom::DoFCS(unsigned short *data, int size) {
  // from Kounine + Koutsenko documentation, p30
  
  //IOutput("DoFCS: data[0]=%x\n",data[0]);
  
  unsigned short fcs=0xffff;
  unsigned char val;
  
  for(int i=0; i<size; i++) { 
    val=data[i]>>8;
    fcs=FCS_Table[(fcs>>8)^val]^(fcs<<8);
    val=data[i] & 0xff;
    fcs=FCS_Table[(fcs>>8)^val]^(fcs<<8);
  }

  //IOutput("DoFCS = %04x\n",fcs);

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

  //IOutput("DoFCS = %04x\n",fcs);

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
  //  const char on[2][5]={"[ ]\0","[X]\0"};
  const char on[2][25]={"\033[1;31m[ ]\033[0m\0","\033[1;32m[X]\033[0m\0"};

  IOutput("\n Summary:\n");

  for (int j=0; j<2; j++) {
    //for (int i=0; i<12; i++) IOutput(" \033[0;%dm%3x:%s\033[0m", (mask&(1<<config[j*12+i]))?32:31, config[j*12+i],on[(mask&(1<<config[j*12+i]))?1:0]);
    for (int i=0; i<12; i++) IOutput(" %3x:%s", config[j*12+i],on[(mask&(1<<config[j*12+i]))?1:0]);
    IOutput("\n");
  }
}

void AMSWcom::ShowReplies(int repl[24]) {
  const int config[24]={0,4,8,0xc,0x10,0x14,0x16,0x12,0xe,0xa,6,2,  1,5,9,0xd,0x11,0x15,0x17,0x13,0xf,0xb,7,3};  
  const char err[5][55]={"---\0", "\033[1;31mABO\033[0m\0","\033[1;36mERR\033[0m\0","\033[1;35mTIM\033[0m\0","\033[1;32mEND\033[0m\0"};

  IOutput("\n Reply status:\n");

  for (int j=0; j<2; j++) {
    //for (int i=0; i<12; i++) IOutput(" \033[0;%dm%3x:%s\033[0m", (mask&(1<<config[j*12+i]))?32:31, config[j*12+i],on[(mask&(1<<config[j*12+i]))?1:0]);
    for (int i=0; i<12; i++) IOutput(" %3x:%s", config[j*12+i],err[repl[config[j*12+i]]]);
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
  
  // for (int sec=0; sec<8; sec++) {
  //   //    IOutput("Sector   File Id   AddrH   EraseS   Stat\n");
  //   IOutput("|   %d    |   %04x  |  %04x |  %04x  | %04x |\n",sec,GetSummary(sec).FileID,GetSummary(sec).AddrH,GetSummary(sec).EraseS, GetSummary(sec).Stat);
  // }
  // IOutput("--------------------------------------------\n");
  // //IOutput("buildstat = 0x%04x\n", GetBuildStat());

  for (int sec=0; sec<8; sec++) {
    // IOutput("Sector   File Id   AddrH   EraseS   Stat\n");
    if (GetSummary(sec).EraseS) IOutput("|   %d    |   %04x  |  %04x |  \033[0;31m%04x\033[0m  | %04x |\n", sec, GetSummary(sec).FileID, GetSummary(sec).AddrH, GetSummary(sec).EraseS, GetSummary(sec).Stat);
    else if (GetSummary(sec).Stat) IOutput("|   %d    |   %04x  |  %04x |  %04x  | \033[0;31m%04x\033[0m |\n", sec, GetSummary(sec).FileID, GetSummary(sec).AddrH, GetSummary(sec).EraseS, GetSummary(sec).Stat);
    else IOutput("|   %d    |   %04x  |  %04x |  %04x  | %04x |\n", sec, GetSummary(sec).FileID, GetSummary(sec).AddrH, GetSummary(sec).EraseS, GetSummary(sec).Stat);
  }
  IOutput("--------------------------------------------\n");
  //IOutput("buildstat = 0x%04x\n", GetBuildStat());

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
  IOutput("AMSWire link ID: %d\n", e1&3);
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


int AMSWcom::SaveFlashFile(char *name) {
  unsigned short length=GetEventSize();
  unsigned short cnt=0;

  unsigned short nseg=Event[0] & 0xf;
  unsigned short seglen=0, start=0, flag=0;

  char filename[200];
  sprintf(filename,"./%s",(name)?name:"file.dat");

  remove(filename);//to avoid strange issues with symblic links present before sendind this command

  FILE *out=fopen(filename,"wt");
  if (out==0) { IOutput("Sorry, file problem...\n"); return 1; }

  fprintf(out,"%04x\n",Event[0]);

  cnt=1;
  
  unsigned short array[EVENTSIZE+1];
  int cnt2=0;

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
    for (int j=0; j<seglen; j++) {
      if (j==0) cnt2=0;
      fprintf(out,"%04x\n",Event[cnt]);
      array[cnt2++]=Event[cnt];
      cnt++;
    }
    
    IOutput("\nsector %d\n",i);
    //    for (int j=0; j<cnt2-1; j++) IOutput("j=%d, val=%x\n",j, array[j]);
    IOutput("fcs=%x with len=%d   last value=%x\n", DoFCS(array,cnt2),cnt2, array[cnt2-1]);
    IOutput("fcs=%x with len=%d   last value=%x\n", DoFCS(array,cnt2-1),cnt2-1, array[cnt2-2]);
    IOutput("fcs=%x with len=%d   last value=%x\n", DoFCS(array,cnt2-2),cnt2-2, array[cnt2-3]);
    IOutput("fcs=%x with len=%d   last value=%x\n", DoFCS(array,cnt2-3),cnt2-3, array[cnt2-4]);
  }

  fclose(out);

  return 0;
}

int AMSWcom::CreateFlashFile(unsigned int amswaddr) {

  char file_par[] = "file.par";
  char file_dat[] = "file.dat";

  FILE *in, *out;
  char chain[MAX_STRN];
  unsigned short fname, fn, flag, length, page, addr, nseg, slen, saddr, nw, type;

  if( (in = fopen(file_par, "r")) == NULL) {
    FOutput("File create: input file %s open error\n", file_par);
    return -1;
  }

  if( (out = fopen(file_dat, "w")) == NULL) {
    FOutput("File create: output file %s open error\n", file_dat);
    return -1;
  }

  if( (out = fopen(file_dat, "w")) == NULL) {
    FOutput("File create: output file %s open error\n", file_dat);
    return -1;
  }

  if ((fgets(chain, MAX_STRN, in)) != NULL) {               // Read file name
    stringbuf sb;
    sb.sputn(chain,strlen(chain));
    string mystr=sb.str();
    istringstream iss(mystr,istringstream::in);
    iss >> hex >> fn;
  }
  else return -1;
  
  fname = fn;
  
  fprintf (out, "%04x\n",fname);
  nseg = fname & 0xF;                                       // number of segments
  type = fname >> 14;  
  
  if (type>1) { IOutput("unknown file type %hx\n", type); return -1; }
  
  for (int i=0; i<nseg; i++) {
    if(fgets(chain, MAX_STRN, in) != NULL) {               // Read segment parameters
      //  FOutput ("0: %04x\n", fn);
      //sscanf(string, "%4x %4x", &slen, &saddr);             // Why fn gets overwriten here?
                                                            // because slen & saddr are short
                                                            // if they were long, there wouldn't
                                                            // be a problem (sscanf() definition).
      //  IOutput("1: %04x %04x, %04x\n", fn, slen, saddr);

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
          if (type==0)
	    if (Event[nw-1] != 0) {                              // last word is for CRC
	      FOutput ("File create: PM segment error - last word is not 0\n");
	      return -1;
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


        }
	else {                                            // CRC error in the AMSW packet
          FOutput ("File create: AMSW error\n");
          //writeDAQMessg(string);
          return -1;
        }
	
      }
      else {                                              // Data memory - 2B wide
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
          
          fprintf(out,"%04x\n",DoFCS(nw-1));

        }
	else {                                            // CRC error in the AMSW packet
          FOutput ("File create: AMSW error\n");
          //writeDAQMessg(string);
          return -1;
        }
	
      }
      
    }
    else {
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
    IOutput("Error while opening file %s\n",name);
    return 1;
  }

  //if ((fgets(strng, MAX_STRN, in)) != NULL)                // Read file name
    fscanf(in, "%4hx", &m);
 // else
 //   return -1;

  fname = m;
  nseg = fname & 0xF; 

  int cnt=0;
  Event[cnt]=fname;
  cnt++;
 
  for (int i=0; i<nseg; i++) {
    //if(fgets(strng, MAX_STRN, in) != NULL) {               // Read segment parameters
      fscanf(in, "%4hx", &slen);
      fscanf(in, "%4hx", &saddr);
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
       //   if(fgets(strng, MAX_STRN, in) != NULL) {         // Read a word
            fscanf(in, "%4hx", &(Event[cnt]));
            cnt++;
     //     } else {
     //       FOutput("FLASH write: no data!");
     //       return 1;
     //     }
        }

      }
      else {                                              // Data memory - 2B wide
        //FOutput("Data memory...\n");
        for (int j=0; j<length; j++) {
        //  if(fgets(strng, MAX_STRN, in) != NULL) {         // Read a word
            fscanf(in, "%4hx", &(Event[cnt]));
            cnt++;
       //   } else {
       //     FOutput("FLASH write: no data!");
       //     return 1;
       //   }
        }
        
      }

    //} else {
    //  FOutput("File write: segment %x has problems\n", i);
    //  return 1;
   // }
  }

  fclose(in); 

  FlashWrite(amswaddr,cnt);
  PrintRXDONE();

  return 0;
}

void AMSWcom::CalibrationRead(unsigned int addr, unsigned short mode, unsigned short param2) {

 if (mode > 1) return;

 int gaus=0;

  IOutput("Getting ped calibration results for address 0x%08x\n",addr);
  //unsigned short mode=0x1f;
  //  unsigned short mode=1;
 

  unsigned short ped[1024], sigr[1024], sigl[1024], sigh[1024], sig[1024], sta[1024], dtrig[1024], gaushis[1024];
  float sig2[1024];

  SlaveMask mask;
  unsigned int slmask=0;
  // testing if we read the calibfiles from many TDRs
  if (addr>>16 == 0x4000) {
    // we read the slave mask
    mask=ReadSlaveMask(addr & 0xffff);
    slmask=mask.ID[0];
    IOutput("slmask=%x\n",slmask);
  }


  for (int i=0; i<1024; i++) {
    ped[i]=0;
    sigr[i]=0;
    sigl[i]=0;
    sig[i]=0;
    sta[i]=0;
    sigh[i]=0;
    dtrig[i]=0;
    gaushis[i]=0;
  }

  unsigned short cn[16], cnm[16];

  unsigned short format=0;

  for (int slave=0; slave<24; slave++) {
    if ((slmask!=0) && ((slmask & (1<<slave)) == 0)) continue;
    if (slmask!=0) addr=(addr & 0xffff) | (((slave<<8) | 0x03f)<<16);

    for (int i=0; i<16; i++) { cn[i]=0; cnm[i]=0; }

    IOutput("Getting calibration data from node 0x%08x\n", addr);
    unsigned short parkeep=GetParameter(addr,0x1000,0x13);
    SetParameter(addr,0x1000,0x13,0x3FF); // we also read the sigma table and 'non-gaussian' channels and the double-trigger values
    CalibRead(addr,mode);
    PrintRXDONE();
    if (mode == 1) {
      unsigned short error=Event[GetEventSize()-2];  // error code
      unsigned short evt_counter=Event[GetEventSize()-3]; // last event read
      if (!((error & 1) || (error & 0x800))) {
	IOutput("calbration not finished yet\n");
	IOutput("%s trigger\n",(error & 2) ? "internal":"external");
	for (int i=1; i<6; i++) if (error &(1<<(i+1))) IOutput("error in step %d\n",i-1);
	if (error & 128) IOutput("unexpected external trigger\n");
	if (error & 256) IOutput("No event in internal trigger mode\n");
	IOutput("calibration events taken until now: %d\n",evt_counter);
	if (error & (1<<13)) IOutput("Calibration aborted\n");
	return;
      }
      
      int cnt=0;
      gaus=0;
      format=Event[cnt++];
      IOutput("format = 0x%x\n",format);
      if (format &   1) { IOutput("reading pedestals\n"); for (int i=0; i<1024; i++) ped[i]=Event[cnt++]; }
      if (format &   2) { IOutput("reading flags\n"); for (int i=0; i<1024; i++) sta[i]=Event[cnt++]; }
      if (format &   4) { IOutput("reading sigma low\n"); for (int i=0; i<1024; i++) sigl[i]=Event[cnt++]; }
      if (format &   8) { IOutput("reading sigma raw\n"); for (int i=0; i<1024; i++) sigr[i]=Event[cnt++]; }
      if (format &  16) { IOutput("reading sigma high\n"); for (int i=0; i<1024; i++) sigh[i]=Event[cnt++]; }
      if (format &  32) { IOutput("reading dt results\n"); for (int i=0; i<1024; i++) dtrig[i]=Event[cnt++]; }
      if (format &  64) { IOutput("reading CN sigma\n"); for (int i=0; i<16; i++) cn[i]=Event[cnt++]; }
      if (format & 128) { IOutput("reading CN sigma mean\n");  for (int i=0; i<16; i++) cnm[i]=Event[cnt++]; }
      if (format & 256) { IOutput("reading gaus results\n"); for (int i=0; i<1024; i++) gaushis[i]=Event[cnt++]; gaus=1;}
      if (format & 512) { IOutput("reading sigma\n"); for (int i=0; i<1024; i++) sig[i]=Event[cnt++]; }
      //  for (int i=0; i<1024; i++) IOutput("%d %d %d\n", i, sigl[i], sigh[i]);
      
      unsigned short version=Event[cnt++];
      unsigned short siglconvS1=Event[cnt++]; // !! factor 8 already included
      unsigned short sighconvS1=Event[cnt++]; // !! factor 8 already included
      unsigned short siglconvS2=Event[cnt++]; // !! factor 8 already included
      unsigned short sighconvS2=Event[cnt++]; // !! factor 8 already included
      unsigned short siglconvK=Event[cnt++]; // !! factor 8 already included
      unsigned short sighconvK=Event[cnt++]; // !! factor 8 already included
      unsigned short sigrconv=Event[cnt++];
      unsigned short badpowerS=Event[cnt++];
      unsigned short badpowerK=Event[cnt++];

      //      IOutput("version = 0x%04x\n", version);
      //      IOutput("siglconvS1 = 0x%04x\n", siglconvS1);
      //      IOutput("sighconvS1 = 0x%04x\n", sighconvS1);
      //      IOutput("siglconvS2 = 0x%04x\n", siglconvS2);
      //      IOutput("sighconvS2 = 0x%04x\n", sighconvS2);
      //      IOutput("siglconvK = 0x%04x\n", siglconvK);
      //      IOutput("sighconvK = 0x%04x\n", sighconvK);      
      //      IOutput("bad power S = %d\n", badpowerS);
      //      IOutput("bad power K = %d\n", badpowerK);

      // now we save the calibration into a file
      
      FILE *calfil, *calfil2;
      char filename[200];
      sprintf(filename,"calib_%d_%08x.cal",param2,addr);
      calfil=fopen(filename,"wt");
      if (calfil==NULL) {
	IOutput("Error: file %s could not be created, perhaps the data dir does not exist ?\n", filename);
	return;
      }
      sprintf(filename,"calib2_%d_%08x.cal",param2,addr);
      calfil2=fopen(filename,"wt");
      if (calfil2==NULL) {
	IOutput("Error: file %s could not be created, perhaps the data dir does not exist ?\n", filename);
	return;
      }  
      for(int i=0; i<16; i++) {
	fprintf(calfil,"%d,   %5.3f,  %5.3f\n",i,((short)cnm[i])/8.0,cn[i]/8.0); // to keep compatibilty with standard cal files
	fprintf(calfil2,"%d,   %5.3f,  %5.3f\n",i,((short)cnm[i])/8.0,cn[i]/8.0); // to keep compatibilty with standard cal files
      }
      
      IOutput("version is 0x%04x\n",version);
      for(int i=0; i<1024; i++) {
	fprintf(calfil2,"%4d %2d %2d %5.3f %5.3f %5.3f %5.3f %4d\n",i, i/64, i%64, ped[i]/8.0, sigr[i]/8.0/sigrconv, sig[i]/8.0, 0.0, sta[i]);
	//IOutput("%4d %2d %2d %5.3f %5.3f %5.3f %5.3f %4d\n",1+i, 1+i/64, 1+i%64, ped[i]/8.0, sigr[i]/8.0/sigrconv, sig[i]/8.0, 0.0, sta[i]);
      }

      
      for(int i=0; i<320; i++) {
	fprintf(calfil,"%4d %2d %2d %5.3f %5.3f %5.3f %5.3f %4d\n",i, i/64, i%64, ped[i]/8.0, sigr[i]/8.0/sigrconv, sigl[i]/1.0/siglconvS1,sigh[i]/1.0/sighconvS1, sta[i]);
	if (sta[i]) IOutput("\033[1;31m%4d %2d %2d %5.3f %5.3f %5.3f %5.3f %4d\033[0m\n",i, i/64, i%64, ped[i]/8.0, sigr[i]/8.0/sigrconv, sigl[i]/1.0/siglconvS1,sigh[i]/1.0/sighconvS1, sta[i]);
      }
      for(int i=320; i<640; i++) {
	fprintf(calfil,"%4d %2d %2d %5.3f %5.3f %5.3f %5.3f %4d\n",i, i/64, i%64, ped[i]/8.0, sigr[i]/8.0/sigrconv, sigl[i]/1.0/siglconvS2,sigh[i]/1.0/sighconvS2, sta[i]);
	if (sta[i]) IOutput("\033[1;31m%4d %2d %2d %5.3f %5.3f %5.3f %5.3f %4d\033[0m\n",i, i/64, i%64, ped[i]/8.0, sigr[i]/8.0/sigrconv, sigl[i]/1.0/siglconvS2,sigh[i]/1.0/sighconvS2, sta[i]);
      }
      for(int i=640; i<1024; i++) {
	fprintf(calfil,"%4d %2d %2d %5.3f %5.3f %5.3f %5.3f %4d\n",i, i/64, i%64, ped[i]/8.0, sigr[i]/8.0/sigrconv, sigl[i]/1.0/siglconvK,sigh[i]/1.0/sighconvK, sta[i]);
	if (sta[i]) IOutput("\033[1;31m%4d %2d %2d %5.3f %5.3f %5.3f %5.3f %4d\033[0m\n",i, i/64, i%64, ped[i]/8.0, sigr[i]/8.0/sigrconv, sigl[i]/1.0/siglconvK,sigh[i]/1.0/sighconvK, sta[i]);
      }
      
      fprintf(calfil,"%04x %04x %04x %04x %04x %04x %04x %04x %04x %04x %04x\n", version, siglconvS1, sighconvS1, siglconvS2, sighconvS2, siglconvK, sighconvK, sigrconv, badpowerS, badpowerK, evt_counter, error);
      fprintf(calfil2,"%04x %04x %04x %04x %04x %04x %04x %04x %04x %04x %04x\n", version, siglconvS1, sighconvS1, siglconvS2, sighconvS2, siglconvK, sighconvK, sigrconv, badpowerS, badpowerK, evt_counter, error);
      IOutput("%04x %04x %04x %04x %04x %04x %04x %04x %04x %04x %04x\n", version, siglconvS1, sighconvS1, siglconvS2, sighconvS2, siglconvK, sighconvK, sigrconv, badpowerS, badpowerK, evt_counter, error);
            
      fclose(calfil); 
      fclose(calfil2); 
      IOutput("%s saved\n",filename);

      sprintf(filename,"calib_%d_%08x.hex",param2,addr);
      calfil=fopen(filename,"wt");
      if (calfil==NULL) {
	IOutput("Error: file %s could not be created, perhaps the data dir does not exist ?\n",filename);
	return;
      }
      
      for(int i=0; i<640; i++) {
	fprintf(calfil,"%4d %2d %2d %04x %04x %04x %04x %04x\n",i, i/64, i%64, ped[i], sigr[i], sigl[i], sigh[i], sta[i]);
      }
      for(int i=640; i<1024; i++) {
	fprintf(calfil,"%4d %2d %2d %04x %04x %04x %04x %04x\n",i, i/64, i%64, ped[i], sigr[i], sigl[i], sigh[i], sta[i]);
	
      }
      fprintf(calfil,"%04x %04x %04x %04x %04x %04x %04x %04x %04x %04x %04x\n", version, siglconvS1, sighconvS1, siglconvS2, sighconvS2, siglconvK, sighconvK, sigrconv, badpowerS, badpowerK, evt_counter, error);
      fclose(calfil);
      
      sprintf(filename,"calibdt_%d_%08x.cal",param2,addr);
      calfil=fopen(filename,"wt");
      if (calfil!=NULL) {
	for(int i=0; i<1024; i++) {
	  fprintf(calfil,"%d  %d\n", i, dtrig[i]);
	}
	fclose(calfil);
      }
      
      if (gaus) {
	sprintf(filename,"calibgaus_%d_%08x.cal",param2,addr);
	calfil=fopen(filename,"wt");
	if (calfil!=NULL) {
	  for(int i=0; i<1024; i++) {
	    fprintf(calfil,"%d  %d\n", i, gaushis[i]);
	  }
	  fclose(calfil);
	}
      }
      
      
      //IOutput("pedestals: ped[1024]=%x   ped[1025]=%x\n",ped[1024],ped[1025]);

      IOutput("Common noise:\n");
      for (int va=0; va<16; va++) IOutput("Va %2d: %5.3f  %5.3f\n",va,((short) cnm[va])/8.0, cn[va]/8.0);
      IOutput("\n");
      
      if (error & 1) IOutput("Calibration finished\n");
      IOutput("%s trigger\n",(error & 2) ? "internal":"external");
      for (int i=1; i<6; i++) if (error &(1<<(i+1))) IOutput("error in step %d\n",i);
      if (error & 128) IOutput("unexpected external trigger\n");
      if (error & 256) IOutput("No event in internal trigger mode\n");
      IOutput("Last event read: %5d\n",evt_counter);
      if (error & (1<<13)) IOutput("Calibration aborted\n");
      if (error & 0x400) IOutput("   flash calibration file write and/or erase failure\n");
      if (error & 0x800) IOutput("   calibration loaded from file\n");
    }
    else {
      // case mode==0
      int cnt=0;
      unsigned short trigger=Event[cnt++];
      unsigned short status=Event[cnt++];
      unsigned short evtreq=Event[cnt++];
      unsigned short evtrec=Event[cnt++];
      unsigned short evtrep=Event[cnt++];
      unsigned short start=Event[cnt++];
      unsigned short stop=Event[cnt++];

      IOutput("Frequency = 0x%04x\n",trigger);
      IOutput("Status = 0x%04x\n", status);
      if (status & 2) IOutput("   sequencer memory failure\n");
      if (status & 4) IOutput("   event length problem\n");
      if (status & 8) IOutput("   event number problem\n");
      if (status & 16) IOutput("   not enough data\n");
      if (status & 32) IOutput("   warning (front end power problem)\n");
      if (status & 0x8000) IOutput("   calibration running\n");
      if (status & 0x4000) IOutput("   calibration results are available\n");
      if (status & 0x2000) IOutput("   calibration failure\n");
      if (status & 0x1000) IOutput("   calibration ready for final processing\n");
      if (status & 0x800) IOutput("   calibration loaded from file\n");
      IOutput("Events requested: %d\n",evtreq);
      IOutput("Events received : %d\n",evtrec);
      IOutput("Events processed: %d\n",evtrep);
      IOutput("Start time: %f sec.\n", start*0.01);
      IOutput("Stop  time: %f sec.\n", stop*0.01);
    }
    SetParameter(addr,0x1000,0x13,parkeep); // we put back the default value

    if (slmask==0) break;
  }

}

void AMSWcom::SaveCalibration(unsigned int addr ,char * fname) {

  IOutput("Collecting and saving data for address 0x%08x\n",addr);

  SetCalParMem(&Prog_3f83);

  FILE *memdump;
  memdump=fopen("calib.mem","wt");
  GetCalibration(addr,memdump);
  fclose(memdump);

  //  GetCalibration(addr,0xf);

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
  if(fname) IOutput("%s saved\n",fname);
  FOutput("calib.cal saved\n");
}

void AMSWcom::LoadCalibrationFile(unsigned int addr,char *file, int calflag) {
  FILE *calfil=fopen(file,"rt");
  if (calfil==NULL) { IOutput("file %s not found\n",file); return; } ;
  double dum,ped[1024],sigr[1024],sig[1024],siglow[1024],sighigh[1024],gain;
  int chan,va,vacha,flag[1024];

  IOutput("calflag=%d\n",calflag);
  //wait();

  memset(&ped,0,sizeof(ped));
  memset(&sig,0,sizeof(sig));
  memset(&siglow,0,sizeof(siglow));
  memset(&sighigh,0,sizeof(sighigh));
  memset(&sigr,0,sizeof(sigr));
  memset(&flag,0,sizeof(flag));

  int i;
  for (i=0; i<16; i++) fscanf(calfil,"%lf,%lf,%lf",&dum,&dum,&dum);
  for (i=0; i<1024; i++) {
    fscanf(calfil,"%d %d %d %lf %lf %lf %lf %d",&chan,&va,&vacha,&ped[i],&sigr[i],&sig[i],&gain,&flag[i]); // reading one line

    IOutput("%d %d %d %lf %lf %lf %lf %d\n",chan,va,vacha,ped[i],sigr[i],sig[i],gain,flag[i]);

    siglow[i]=Prog_3f83.SigLFactor*sig[i];
    sighigh[i]=Prog_3f83.SigHFactor*sig[i];    
    sigr[i]*=Prog_3f83.SigrFactor;
    //if ((i>130) && (i<(130+200))) ped[i]-=200;
    //if ((i>730) && (i<(730+200))) ped[i]-=200;

    ped[i]*=Prog_3f83.PedFactor;
    // now we create fake status
    //if ((va==1 || va==3 || va==7 || va==10 || va==14 || va==16) && (vacha<33+va)) flag[i]=1;
    //if (vacha<32) flag[i]=1;
  }
  fclose(calfil);

  if (calflag & kPEDS) {
    IOutput("writing pedestals... ");
    WritePMCalib(addr,Prog_3f83.Ped,ped,1024,0);
  }
  if (calflag & kSIGH) { 
    IOutput("writing sigH... ");
    WritePMCalib(addr,Prog_3f83.SigH,sighigh,1024,0);
  }
  if (calflag & kSTAT) {
    IOutput("writing stats... ");
    WritePMCalib(addr,Prog_3f83.Stat,flag,1024,1);
  }
  if (calflag & kSIGR) {
    IOutput("writing sigma raw... ");
    WriteDMCalib(addr,Prog_3f83.Sigr,sigr,1024);
  }
  if (calflag & kSIGL) {
    IOutput("writing sigL... ");
    WriteDMCalib(addr,Prog_3f83.SigL,siglow,1024);
  }
  
}

void AMSWcom::CommandLV1(int ntrig) {
  int cnt=0;
  while (1) {
    Command_Lv1();
    if (ntrig!=-1) {
      cnt++;
      usleep(1000);
      if (cnt>=ntrig) break;
    }
  }
}

int AMSWcom::ProcessHexCommand(unsigned int addr,ushort cmd,int nparam, ushort * param){
  double elapsed=0;
  struct timeval tv1,tv2;

  switch (cmd) {
  case cAMSW_SLAVTST:
    SlaveTest(addr);
    break;
  case cAMSW_EVNTGET:
    if (nparam==0) GetEvent(addr);
    else if (nparam==1) GetEvent(addr,param[0]);
    else if (nparam==2) GetEvent(addr,param[0], param[1]);
    //    GetEvent(addr);
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
    //  case cAMSW_CALRED:
    //    GetCalibration(addr,param[0]);
    //    break;
  case cAMSW_CALPED:
    if (nparam==1) Calibrate(addr,param[0]);
    else if (nparam==2) Calibrate(addr,param[0], param[1]);
    break;
    //    if(param[0]==1)
    //    Calibrate(addr,param[0]);
    //    else
    //    Calibrate(addr,param[0],param[1]);
    //    break;
  case cAMSW_CALDAC:
    if (param[0]==0) CalibrateDac(addr,param[1]);
    else {
      if (nparam==3) SDProc(addr, param[0], param[1], param[2]);
      if (nparam==2) SDProc(addr, param[0], param[1]);
      if (nparam==1) SDProc(addr, param[0]);
    }
    break;
    //    CalibrateDac(addr,param[0]);
    //    break;
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
  case cAMSW_HKINFO:
    HKRead(addr);
    break;
  case cAMSW_GETSTAT:
    PrintNodeStatus( addr);
    break; 
  case cAMSW_SSFRD:
    GetSSF( addr);
    break;
  case cAMSW_SSFWR:
    SetSSF(addr, param[0]);
    break;
  case cAMSW_SDPROC_RD:
    SDProcRead(addr, param[0]);
    break;
  case cAMSW_CALPEDRD:
    CalibrationRead(addr, param[0], param[1]);
    break;
  case cAMSW_BUSYLD:
    WriteBusyMask(addr, param[0], param[1]);
    break;
  case cAMSW_BUSYRD:
    ReadBusyMask( addr);
    break;
  case cAMSW_BUSYSTAT:
    ReadBusyStat( addr);
    break;
  case cAMSW_LV1:
    CommandLV1(param[0]);
    break;
  case cAMSW_QLISTRD:
    ReadQList( addr);
    break;
  case cAMSW_WRITE_PED:
    LoadCalibrationFile(addr, "calload.cal", kPEDS);
    break;
  case cAMSW_WRITE_FILE_DM:
    WriteDM(addr, "DataMem.mem");
    break;
  case cAMSW_WRITE_CAL_PAR:
    WriteCalibParam(addr, "calib.par");
    break;
  case cAMSW_WRITE_LASER_PAR:
    WriteCalibParam(addr, "laser.par");
    break;
  case cAMSW_SDCONFIG:
    SetParameter(addr, param[0], param[1]);
    break;
  case cAMSW_RDCONFIG:
    GetParameter(addr, param[0]);
    break;
  case cAMSW_FILECR:
    CreateFlashFile(addr);
    break;
  case cAMSW_COMMAND:
    for(int ii=0;ii<nparam;ii++) IOutput("0x%04x",param[ii]);
    IOutput("\n");
    gettimeofday(&tv1,0);
    Command(nparam,param);
    gettimeofday(&tv2,0);
    elapsed=(tv2.tv_sec-tv1.tv_sec)*1000+ (tv2.tv_usec-tv1.tv_usec)/1000;
    IOutput("Elapsed time is %6.2f seconds\n",elapsed/1000.);
    PrintRXDONE();
    PrintEvent();
    IOutput("size = %d\n", GetEventSize());
    PrintBuildStat(GetBuildStat());
    break;
  case cAMSW_GCOMMAND:
    for(int ii=0;ii<nparam;ii++) IOutput("0x%04x",param[ii]);
    IOutput("\n");
    gettimeofday(&tv1,0);
    Command2(addr,param[0],nparam-1,&(param[1]));
    gettimeofday(&tv2,0);
    elapsed=(tv2.tv_sec-tv1.tv_sec)*1000+ (tv2.tv_usec-tv1.tv_usec)/1000;
    IOutput("Elapsed time is %6.2f seconds\n",elapsed/1000.);
    PrintRXDONE();
    PrintEvent();
    IOutput("size = %d\n", GetEventSize());
    PrintBuildStat(GetBuildStat());
    break;
  case cAMSW_RCOMMAND:
    for(int ii=0;ii<nparam;ii++) IOutput("0x%04x",param[ii]);
    IOutput("\n");
    gettimeofday(&tv1,0);
    RAWCommand(addr,nparam,&(param[0]));
    gettimeofday(&tv2,0);
    elapsed=(tv2.tv_sec-tv1.tv_sec)*1000+ (tv2.tv_usec-tv1.tv_usec)/1000;
    IOutput("Elapsed time is %6.2f seconds\n",elapsed/1000.);
    PrintRXDONE();
    PrintEvent();
    PrintBuildStat(GetBuildStat());
    IOutput("size = %d\n", GetEventSize());
    break;
  case cAMSW_TEST:
    SaveJLV1Conf(addr);
    break;
  case cAMSW_TEST2:
    WriteJLV1Conf(addr);
    break;
//   case cAMSW_WRTCONF:
//     Command2(addr, 0x49, nparam, &(param[0]));
//     break;
//   case cAMSW_RDCONF:
//     Command2(addr, 0x09, nparam, &(param[0]));
//     PrintEvent();
//     break;
  default:
    IOutput("Command not implemented yet, use ff10, ffff or fffe\n");
    IOutput("ff10 for the rawcommand a la Philipp\n");
    IOutput("ffff for rawcommand\n");
    IOutput("fffe for true rawcommand\n");
    return -2;
  }

  return 0;
}

unsigned short AMSWcom::SaveJLV1Conf(unsigned int selfaddr){

  unsigned short array[37]=
    {0x6024,  
     0x0100,  0x0101,  0x0102,  0x0103,  0x0104,  0x0105,  
     0x0106,  0x0107,  0x0108,  0x0109,  0x010a,  0x010b,
     0x010c,  0x010d,  0x010e,  0x010f,  0x0110,  0x0111,
     0x0112,  0x0113,  0x0114,  0x0115,  0x0116,  0x0117,
     0x0118,  0x0119,  0x011a,  0x011b,  0x011c,  0x011d,
     0x011e,  0x011f,  0x0200,  0x0201,  0x0202,  0x0300
    };
  
  Command2(selfaddr,0x09,37,array);
 
  PrintRXDONE();
  PrintEvent();
  
  char filename[200];
  sprintf(filename,"JLV1_conf.dat");

  FILE *out=fopen(filename,"wt");
  if (out==0) { IOutput("Sorry, file problem...\n"); return 1; }

  fprintf(out,"%04x \n",Event[0]);
  for(int ii=1;ii<EventSize-1;ii++){
    fprintf(out,"%04x ",Event[ii]);
    if(ii%2==0)  fprintf(out," \n");
  }
  fclose(out);
  return EventSize ;

}


unsigned short AMSWcom::WriteJLV1Conf(unsigned int selfaddr){
  unsigned short array[300];
  char filename[200];
  sprintf(filename,"conf_JLV1-2.dat");
  ushort  a;
  ushort cnt=0;
  ushort npar=5;
  a=0x49;
  array[0]=0x6000&npar;  //number of parameters
  array[1]=0x010b;   array[2]=0x0002;  //LUT-FT0
  array[3]=0x010c;   array[4]=0x0000;  //LUT-FT1
  array[5]=0x010d;   array[6]=0x0000;  //LUT-BZ
  array[7]=0x010f;   array[8]=0x22fe;  //Sub-Lvl1 mask(0-7) and acc mask 
  array[9]=0x0118;   array[10]=0x03ff;  //Sub-Lvl1 (0) prescaler 


  Command(selfaddr,0x49,3,0x6001,0x010b,0x0002);  //LUT-FT0
  Command(selfaddr,0x49,3,0x6001,0x010c,0x0000);  //LUT-FT1
  Command(selfaddr,0x49,3,0x6001,0x010d,0x0000);  //LUT-BZ
  Command(selfaddr,0x49,3,0x6001,0x010f,0x22fe); //Sub-Lvl1 mask(0-7) and acc mask 
  Command(selfaddr,0x49,3,0x6001,0x0118,0x03ff); //Sub-Lvl1 (0) prescaler 



  //Command2(selfaddr,a,npar*2+1,array);
  // FILE *out=fopen(filename,"r");
//   if (out==0) { IOutput("Sorry, file problem... with %s \n I give up! \n",filename); return 1; }
//   while(!feof(out)){
//     fscanf(out,"%04hx",&a);
//     array[cnt]=a;
//     cnt++;
//   }
//   fclose(out);
//    IOutput("Read %d words\n",cnt);
//    for (int ii=0;ii<cnt-1;ii++){
//       IOutput("%04x ",array[ii]);
//       if(ii%13==0)  IOutput(" \n");
//    }
//   IOutput(" \n  Sending Conf to JLV1 ... \n");
//   Command2(selfaddr,0x0F,cnt,array);

//    PrintRXDONE();
//     PrintEvent();
//   sprintf(filename,"conf_JLV1-1_busy.dat");
  
//   cnt=0;
//   out=fopen(filename,"r");
//   if (out==0) { IOutput("Sorry, file problem... with %s \n I give up! \n",filename); return 1; }
//   while(!feof(out)){
//     fscanf(out,"%04hx",&a);
//     array[cnt]=a;
//     cnt++;
//   }
//   fclose(out);
//   IOutput("Read %d words\n",cnt);
//   // for (int ii=0;ii<cnt;ii++){
// //     IOutput("%04x ",array[ii]);
// //     if(ii%13==0)  IOutput(" \n");
// //   }
// IOutput(" \n  Sending Busy mask to JLV1 ... \n");
//   Command2(selfaddr,0x0F,cnt,array);
//  PrintRXDONE();
//     PrintEvent();
  return cnt;
}

int AMSWcom::WriteJLV1BusyMask(unsigned int addr, ushort busy0x12, ushort busy0x13, ushort busy0x14){

  int ret=0;

  Command(addr, 0x49, 3, 0x6001, 0x0103, busy0x12);  //BUSY/TRST Mask 0x12
  ret+=GETRXDONE();
  Command(addr, 0x49, 3, 0x6001, 0x0104, busy0x13);  //BUSY Mask 0x13
  ret+=GETRXDONE();
  Command(addr, 0x49, 3, 0x6001, 0x0105, busy0x14);  //BUSY Mask 0x14
  ret+=GETRXDONE();

  return ret;  
}

void AMSWcom::TranslateState(int crateno, unsigned short var, unsigned short cont) {
  char groupn[8][20]={
    "S9011AT\0", "TPSFE 4\0", "TPSFE 6\0", "TPSFE 14\0", "TPSFE 16\0", "TBS 5\0", "TBS 15\0", "ACTEL\0" 
  };
  
  // S9011AT
  char s9011[5][8][50];
  memset(s9011,'\0',sizeof(s9011));
  
  for (int i=0; i<8; i++) sprintf(s9011[1][i],"S9051_%d %s",(i/2),(i%2)?"K":"S");
  
  for (int i=0; i<4; i++) sprintf(s9011[2][i],"S9053_%d %s",(i/2),(i%2)?"Cold":"Hot");
  
  for (int i=0; i<4; i++) sprintf(s9011[3][i],"S9055_%d %s",(i/2),(i%2)?"Cold":"Hot");
  
  sprintf(s9011[4][0],"Error word\0");
  sprintf(s9011[4][1],"Over-current word\0");
  sprintf(s9011[4][2],"Trip bits\0");

  //for (int i=0; i<5; i++) for (int j=0; j<8; j++) IOutput("%04x: %s\n", (2<<12)+(i<<8)+j,s9011[i][j]);
  
  // TPSFE
  char tpsfe[9][7][50];
  memset(tpsfe,'\0',sizeof(tpsfe));
  
  for (int i=0; i<6; i++) sprintf(tpsfe[1][i],"TDR %d control",i);
  sprintf(tpsfe[1][6],"TDR auto-off mode");
  
  for (int i=0; i<6; i++) sprintf(tpsfe[2][i],"LRS %d control",i);
  sprintf(tpsfe[2][6],"LRS auto-off mode");
  
  for (int i=0; i<6; i++) sprintf(tpsfe[3][i],"LRK %d control",i);
  sprintf(tpsfe[3][6],"LRK auto-off mode");
  
  sprintf(tpsfe[4][0],"Error word");
  
  for (int i=0; i<6; i++) sprintf(tpsfe[5][i],"TDR %d trip counter",i);
  
  for (int i=0; i<6; i++) sprintf(tpsfe[6][i],"LRS %d trip counter",i);
  
  for (int i=0; i<6; i++) sprintf(tpsfe[7][i],"LRK %d trip counter",i);
  
  sprintf(tpsfe[8][0],"Actel trip counter");
  
  //for (int i=0; i<9; i++) for (int j=0; j<7; j++) IOutput("%04x: %s\n", (3<<12)+(i<<8)+j,tpsfe[i][j]);
  
  
  // TBS
  char tbs[3][8][50];
  memset(tbs,'\0',sizeof(tbs));
  
  for (int i=0; i<4; i++) sprintf(tbs[1][i],"LR %d %s control",i/2, (i%2)?"Cold":"Hot");
  for (int i=4; i<8; i++) sprintf(tbs[1][i],"LR %d %s 80V mode",(i-4)/2, (i%2)?"Cold":"Hot");
  
  sprintf(tbs[2][0],"Error word 0");
  sprintf(tbs[2][1],"Error word 1");
  
  //for (int i=0; i<3; i++) for (int j=0; j<8; j++) IOutput("%04x: %s\n", (7<<12)+(i<<8)+j,tbs[i][j]);
  
  // ACTEL
  char crate[50];
  sprintf(crate,"Actel mask");
  
  // check
  int group=(var>>12)-2;
  int subg=(var & 0x0F00)>>8;
  int name=var & 0xff;
  
  IOutput("Crate %d ",crateno);
  
  if (group==0) {
    IOutput("%s: %s is %x\n",groupn[group],s9011[subg][name],cont);
  }
  else if (group<5) {
    IOutput("%s: %s is %x\n",groupn[group],tpsfe[subg][name],cont);
  }
  else if (group<8) {
    IOutput("%s: %s is %x\n",groupn[group],tbs[subg][name],cont);
  }
  else if (group==8) {
    IOutput("%s: %s is %x\n",groupn[group],crate,cont);
  }
  else {
    IOutput("unknown group %d\n", group);
  }
  
}

void AMSWcom::InitCalParMem() {
  //---------------- Program 3123 ---------------------
  Prog_3123.Ped=0x1300;
  Prog_3123.MemPed=kDSP_PM;

  Prog_3123.SigH=0x1b04;
  Prog_3123.MemSigH=kDSP_PM;

  Prog_3123.SigL=0x1b04;
  Prog_3123.MemSigL=kDSP_PM;

  Prog_3123.Stat=0x1702;
  Prog_3123.MemStat=kDSP_PM;
  Prog_3123.Sigr=0x1701;
  Prog_3123.MemSigr=kDSP_DM;

  Prog_3123.SigHFactor=1.0;
  Prog_3123.SigLFactor=1.0;

  //-------------------- Program 3ed3 ---------------------
  Prog_3ed3.Ped=0x1300;
  Prog_3ed3.MemPed=kDSP_PM;
  
  Prog_3ed3.SigH=0x1b04;
  Prog_3ed3.MemSigH=kDSP_PM;
  
  Prog_3ed3.SigL=0x1b04;
  Prog_3ed3.MemSigL=kDSP_DM;

  Prog_3ed3.Stat=0x1702;
  Prog_3ed3.MemStat=kDSP_PM;

  Prog_3ed3.Sigr=0x1700;
  Prog_3ed3.MemSigr=kDSP_DM;

  Prog_3ed3.SigHFactor=3.5;
  Prog_3ed3.SigLFactor=2.0;

  //-------------------- Program 3f83 ---------------------
  Prog_3f83.Ped=0x1300;
  Prog_3f83.MemPed=kDSP_PM;

  Prog_3f83.SigH=0x1b04;
  Prog_3f83.MemSigH=kDSP_PM;
  
  Prog_3f83.SigL=0x3200;
  Prog_3f83.MemSigL=kDSP_DM;

  Prog_3f83.Stat=0x1702;
  Prog_3f83.MemStat=kDSP_PM;

  Prog_3f83.Sigr=0x3602;
  Prog_3f83.MemSigr=kDSP_DM;

  Prog_3f83.SigHFactor=8*3.5; //3.5; 
  Prog_3f83.SigLFactor=8*2.0; //2.0;

  Prog_3f83.SigrFactor=8*10.0; // precision*real_factor

  Prog_3f83.PedFactor=8.0; // precision

  //----------------------- calib 3710_07 ----------------

  Prog_3710_07.Ped=0x1300;
  Prog_3710_07.MemPed=kDSP_PM;

  Prog_3710_07.SigH=0x1b04;
  Prog_3710_07.MemSigH=kDSP_PM;
  
  Prog_3710_07.SigL=0x1b04;
  Prog_3710_07.MemSigL=kDSP_DM;

  Prog_3710_07.Stat=0x0e00;
  Prog_3710_07.MemStat=kDSP_PM;

  Prog_3710_07.Sigr=0x1702;
  Prog_3710_07.MemSigr=kDSP_DM;

  Prog_3710_07.SigHFactor=8*3.5; //3.5; 
  Prog_3710_07.SigLFactor=8*2.0; //2.0;

  Prog_3710_07.SigrFactor=8*10.0; // precision*real_factor

  Prog_3710_07.PedFactor=8.0; // precision
}

void AMSWcom::Reorganize(unsigned short *array) {
  int cnt=0;
  
  unsigned short temp[1024];
  
  for (int i=0; i<320; i++) {
    temp[i]=array[3*i];
    temp[320+i]=array[3*i+1];
    temp[640+i]=array[3*i+2];
  }
  
  for (int i=0; i<64; i++) temp[640+320+i]=array[640+320+i];
  
  for (int i=0; i<1024; i++) array[i]=temp[i];
  
}

void AMSWcom::InterpretData() {
  // for now, we assume we are considering a TDR
  
  //  char file[200];
  //  sprintf(file,"calib_0_0000ffff.hex");
  //  FILE *calfil=fopen(file,"rt");
  //  if (calfil==NULL) { IOutput("file %s not found\n",file); return; } ;
  //  unsigned short ped[1024],sigr[1024],sig[1024],siglow[1024],sighigh[1024];
  //  int chan,va,vacha,flag[1024];
  //
  //  memset(&ped,0,sizeof(ped));
  //  memset(&sig,0,sizeof(sig));
  //  memset(&siglow,0,sizeof(siglow));
  //  memset(&sighigh,0,sizeof(sighigh));
  //  memset(&sigr,0,sizeof(sigr));
  //  memset(&flag,0,sizeof(flag));
  //
  //  int i;
  //  for (i=0; i<1024; i++) {
  //    fscanf(calfil,"%d %d %d %hx %hx %hx %hx %hx",&chan,&va,&vacha,&ped[i],&sigr[i],&siglow[i],&sighigh[i],&flag[i]); // reading one line
  //  }
  //  fclose(calfil);

  unsigned short err[24];
  memset(err,0,sizeof(err)); 
  unsigned short length=GetEventSize();
  
  if (!length) return;
  
  unsigned short status=Event[length-1];
  int notgroup = status & (1<<5);
  
  IOutput("%s\n", (notgroup)?"no sub-structure":"group assembly");
  
  int cnt=0;
  unsigned short evtnum=Event[cnt++];
  
  IOutput("event number: %d\n",evtnum);
  
  if (notgroup) {
    PrintCluster(length-1, cnt);
  }
  else {
    unsigned short jinfstat=Event[length-1];
    
    IOutput("event number: %d, jinflength = %d, jinfstat = 0x%04x\n",evtnum, length, jinfstat);
    unsigned short tdrlength=0;
    unsigned short tdrstat=0;
    
    //if (!(jinfstat & (1<<9))) { // here we check that there is no buildstat problem
    do { 
      tdrlength=Event[cnt];
      tdrstat=Event[cnt+tdrlength];
      
      IOutput("TDRlength=%x, TDRstat = 0x%04x\n",tdrlength, tdrstat);
      if (tdrlength==1) err[tdrstat & 0x1f]=1;
      if (tdrlength==4) err[tdrstat & 0x1f]=2;
      PrintCluster(tdrlength, cnt+1);
      cnt+=tdrlength+1;
    } while (cnt<length-3);
    
    //IOutput("cnt = %d, event = %x\n", cnt, Event[cnt]);
    unsigned int crate=(Event[cnt++])<<16;
    crate|=(Event[cnt++]);
    ShowEvtRepl(crate, err);
    IOutput("crate status = 0x%x\n",crate);
    //for (int tdr=0; tdr<24; tdr++) {
    //  if (!(crate & (1<<tdr))) IOutput("TDR %02x replied\n",tdr);
    //}
  }
  //}
  
}

void AMSWcom::ShowEvtRepl(unsigned int stat, unsigned short err[24]) {
  const int config[24]={0,4,8,0xc,0x10,0x14,0x16,0x12,0xe,0xa,6,2,  1,5,9,0xd,0x11,0x15,0x17,0x13,0xf,0xb,7,3};  
  const char sta[4][55]={"[ ]\0", "\033[1;32m[*]\033[0m\0", "\033[1;31mERR\033[0m\0","\033[1;31m[E]\033[0m\0"};

  IOutput("\n Reply:\n");
  
  for (int j=0; j<2; j++) {
    for (int i=0; i<12; i++) {
      int tdr=config[j*12+i];
      IOutput(" %3x:%s", tdr, sta[ (err[tdr]==1)? 2 : ( (err[tdr]==2)? 3 : (((1<<tdr) & stat)? 0 : 1 ))  ]);
    }
    IOutput("\n");
  }
}

void AMSWcom::PrintCluster(unsigned short length, unsigned short cnt) {
  unsigned short status=0;
  char msg[100];
  if (length==1) {
    status=Event[cnt];
    IOutput("Node address 0x%04x: ", status & 0x1f);
    if ((status & 0xff00) == 0x1000) { sprintf(msg,"\033[0;31mABORT\033[0m"); }
    if ((status & 0xff00) == 0x1800) { sprintf(msg,"\033[0;31mERROR\033[0m"); }
    if ((status & 0xff00) == 0x2800) { sprintf(msg,"\033[0;31mTIMEOUT\033[0m"); }
    if ((status & 0xff00) == 0x3000) { sprintf(msg,"\033[0;31mAMSW ERR\033[0m"); }
    if ((status & 0xff00) == 0x2000) { sprintf(msg,"\033[0;32mEND\033[0m"); }
    IOutput("%s\n\n",msg);
    if (status & 0x0400) IOutput("\033[1;31mFE power problem\033[0m\n");
    if (status & 0x0200) IOutput("\033[1;31mbuild errors\033[0m\n");
    if (status & 0x0100) IOutput("cumulative status set to 1\n");
    if (status & 0x0080) IOutput("compressed data\n");
    if (status & 0x0040) IOutput("raw data\n");
    if (status & 0x0020) IOutput("no sub-structure\n");
    else IOutput("group assembly\n");
    IOutput("node address 0x%04x\n", status & 0x1f);
    
    IOutput("\n");
    return;
  }
  

  unsigned short nodestat=Event[cnt+length-1];

  //if (!(nodestat>>15)) { IOutput("No DATA\n\n"); return; }
 
  unsigned short sta=((nodestat>>11) & 0xf);

  if (sta) { // errors
    switch (sta) {
    case 1:
      sprintf(msg,"First received packet has no BC1 bit set");
      break;
    case 2:
      sprintf(msg,"No room to append BC=11 packet");
      break;
    case 3:
      sprintf(msg,"No valid data");
      break;
    case 4:
      sprintf(msg,"Event number mismatch");
      break;
    case 5:
      sprintf(msg,"BC=11 packet with CRC error");
      break;
    case 6:
      sprintf(msg,"Timeout on the reply to NEXT command");
      break;
    case 7:
      sprintf(msg,"0-length reply to NEXT command");
      break;
    case 8:
      sprintf(msg,"Intermediate or last packer has wrong BC bit settings");
      break;
    case 9:
      sprintf(msg,"No buffers available for multi-packet transaction");
      break;
    case 0xa:
      sprintf(msg,"No room to append BC=00/01 packet");
      break;
    case 0xb:
      sprintf(msg,"Event number mismatch, multi-packet");
      break;
    case 0xc:
      sprintf(msg,"Multi-packet reply with CRC error");
      break;
    }
    IOutput("\033[1;31m%s\033[0m\n\n",msg);
    return;
  }

  int raw=nodestat & (1<<6);
  int red=nodestat & (1<<7);

  int stop=cnt+length-1;

  if (raw) {
    IOutput("\033[0;07mSkipping raw data\033[0m\n");
    cnt+=1024;
  }


  if (red) while (cnt<stop) {
      unsigned short clen=Event[cnt++];
      unsigned short clsn=clen>>7;
      
      clen &= 0x7f;
      clen++;
      unsigned short start=Event[cnt++];
      unsigned short cnstat=(start&0xfc00);
      start&=0x3ff;
      IOutput("cluster start:  %d\n", start);
      IOutput("cluster lenght: %d\n", clen);
      IOutput("cluster sn    : %5.2f\n", clsn*0.25);
      IOutput("cn stat       : 0x%x\n", cnstat);
      
      short int maxval=0, maxpos=0;
      for (int i=0; i<clen; i++) {
	short int cont=(short int)Event[cnt++];
	//IOutput("content channel %3d: %d  sn:%5.2f (flag=%d)\n", start+i, cont, cont/1.0/siglow[start+i],flag[start+i]);
	IOutput("content channel %3d: %d\n", start+i, cont);
	//      if (!flag[i+start] && (cont>maxval)) { 
	//	maxval=cont;
	//	maxpos=i+start;
	//      }
      }
      //IOutput("max cont=%d, truncated=%d , channel %d\n", maxval, maxval & 0x1ff, maxpos);
      //IOutput("expected sn: %5.2f  measured sn: %5.2f  sigmalow=%d (0x%04x)\n\n",maxval/1.0/(siglow[maxpos]),clsn*0.25, siglow[maxpos], siglow[maxpos]); 
      IOutput("\n");
    }
  

  
  IOutput("\nnode status: 0x%04x\n", nodestat);
  if (nodestat & 0x0400) IOutput("\033[1;31mFE power problem\033[0m\n");
  if (nodestat & 0x0200) IOutput("\033[1;31mbuild errors\033[0m\n");
  if (nodestat & 0x0100) IOutput("cumulative status set to 1\n");
  if (nodestat & 0x0080) IOutput("compressed data\n");
  if (nodestat & 0x0040) IOutput("raw data\n");
  if (nodestat & 0x0020) IOutput("no sub-structure\n");
  else IOutput("group assembly\n");
  IOutput("node address 0x%04x\n", nodestat & 0x1f);
  
  IOutput("\n"); 
}

void AMSWcom::ShowGroupReplies() {

  unsigned int cnt=0, len=0, status=0, saddr=0;
  char msg[50];

  int repl[24];
  memset(&repl,0,sizeof(repl));

  do {
    len=Event[cnt];
    cnt+=len;
    status= Event[cnt++];
    sprintf(msg," ");
    saddr = status & 0x1f;
    status &= 0xffe0;	  
    //IOutput("Address 0x%02x3f has status 0x%04x", saddr, status);
    if (status == 0x1000) { repl[saddr] = 1; sprintf(msg,"\033[0;31mABORT\033[0m"); }
    if (status == 0x1800) { repl[saddr] = 2; sprintf(msg,"\033[0;31mERROR\033[0m"); }
    if (status == 0x2800) { repl[saddr] = 3; sprintf(msg,"\033[0;31mTIMEOUT\033[0m"); }
    if (status == 0x2000) { repl[saddr] = 4; sprintf(msg,"\033[0;32mEND\033[0m"); }
    //IOutput(" %s\n",msg);
  } while (cnt+1<GetEventSize());
  
  ShowReplies(repl);  
}

void AMSWcom::textcolor(int attr, int fg, int bg) {
  char command[13];
  
  /* Command is the control command to the terminal */
  sprintf(command, "%c[%d;%d;%dm", 0x1B, attr, fg + 30, bg + 40);
  IOutput("%s", command);
}

void AMSWcom::resetcolor() {
  IOutput("\033[0m");
}
