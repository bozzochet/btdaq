#ifndef __AMSWCOM_HPP__
#define __AMSWCOM_HPP__

#include "QList.h"
#include <string>
#include <sys/types.h> // needed to add this to compile on Fedora10

#define    SUCCESS        0
#define    ERROR          1

#define MAX_STRN 0x100

typedef unsigned short ushort;
typedef unsigned int uint;

//22/08.2007 pzuccon: Import from TestPCI: all the printout for the interactive use
//                                         the method to call a command from its hex code
//                                         update the output redirection method via a pointer to a function
//

// // P.Zuccon Aug-07 trasform this macro into a regular function
// // #define    CHECK_FOR_EPPUP                                                                                     \
// //                                                                                                                \
// // do {                                                                                                           \
// //     if( eppUp == false ) {                                                                                     \
// //         if( Init( ) == ERROR ) {                                                                               \
// //             FOutput( COL_STRONG "Unable to get control of the EPP: " COL_EVIDENT "%s\n", strerror(errno));    \
// //             return -1000;                                                                                            \
// //         }                                                                                                      \
// //     }                                                                                                          \
// // } while ( 0 )


#define    PROGMEMORY      0
#define    DATAMEMORY      1
#define    EVENTSIZE       16384
#define    CMDSIZE         300

typedef void (*OUTFUN) (std::string s);

enum sCOMHARDWARE {
  kAMSW_EPP=0,
  kAMSW_PCI=1,
};

enum sSESSIONTYPE {
  kBATCH=0,
  kINTER=1,
};


enum sDSPMEMORY {
  kDSP_PM=0,
  kDSP_DM=1,
};

typedef struct FlashList {
  unsigned short FileID;
  unsigned short AddrH;
  unsigned short EraseS;
  unsigned short Stat;
} FlashList;


typedef struct NodeStatus {
  unsigned short Ver;
  unsigned short Attr;
  unsigned short SubdVer;
  unsigned short LStime;
  unsigned short MStime;
  unsigned short NodeStat;
  unsigned short LastEvt;
  unsigned short NBuildErr;
  unsigned short NAMSWerr;
  unsigned short NFlashErr;
  unsigned short ReplyStat;
} NodeStatus;


typedef struct CalibMemory {
  unsigned short Ped;  // address
  int MemPed;          // memory type
  unsigned short SigH; // address
  int MemSigH;         // memory type
  unsigned short SigL; // address
  int MemSigL;         // memory type
  unsigned short Stat; // address
  int MemStat;         // memory type
  unsigned short Sigr; // address
  int MemSigr;         // memory type
  double SigHFactor;   // Factor for SigH
  double SigLFactor;   // Factor for SigL
  double SigrFactor;   // Factor for Sigr
  double PedFactor;   // Factor for Ped
} CalibMemory;


typedef struct SlaveMask {
  int Nmask;
  unsigned int ID[16];
} SlaveMask;


class AMSWcom
{

  //------------ PRIVATE -------------------//
 private:
  
  // * eppUp is common to all the class (static)
  static bool eppUp;
  // * can be 0-3
  int port;
  int pcicard;
  int hwtype; // can be kAMSW_EPP or kAMSW_PCI
  int sType; // can be kBATCH or kINTER
  unsigned short RX_DONE;
  int TimeOut;
  unsigned short BuildStat;
  unsigned short CRC;
  FlashList FlashSummary[8];
  NodeStatus NStat;
  int DEBUG;
  unsigned short CmdArray[CMDSIZE];  
  unsigned char cmdch[2*EVENTSIZE+10];
  unsigned short FCS_Table[256];
  unsigned short DataSize;
  OUTFUN  Output;

  //  int Init_commandEPP   (unsigned short addr, unsigned short cmd, int args, ... );


  int SendCommandPCI(int link, const u_char command[], int csize, u_short wordarray[], int *datasize, int mode24=0);
  //  int SendCommandPCI24(int link, const u_char command[], int csize, u_short wordarray[], int *datasize);
  void byte_to_word(u_char bytearray[], int *size, u_short wordarray[]);
  void byte24_to_word(u_char bytearray[], int *size, u_short wordarray[]);
  void Build_stat();
  // unsigned short Print_len( char * command );
  
   

  // * something useful for epp routines
  unsigned short rx_status  ( );
  unsigned short rx_len     ( );
  void           tx_start   ( unsigned short addr, unsigned short stat );
  void           bmrd_init  ( );
  void           bmwr_init  ( );
  void           crcrd_init ( );
  unsigned short crcrd_end  ( );
  void           crcwr_init ( );
  unsigned short crcwr_end  ( );
  unsigned short rx_done ( );
  unsigned short busy_stat(void);
  void lv1_start(void);
  
  void PrepareFCSTable();

  int Check_for_EPPUP();
  // * when getting event this will contains how many valid events there will be in `double event[1024]`
  int EventSize;
  

  //----------------- PUBLIC --------------------------//

 public:
  // * we allow the user to define its own output function:
  // * FOutput() or IOutput() will be called from every inner method, it will
  // * format the output and will send it to the "out" function as a variable of C++ string type
  // * the function must be of the type OUTFUN. As default the output function is to stdout.
  // * Using the SetOuput() method it is possible to set any Output function.

  void SetOutput( OUTFUN );
  static void OutputStd  ( std::string s );  
  static void OutputFile ( std::string s );
  void FOutput ( const char * fmt, ... ); // gcc 4.3 : needed to add const
  void IOutput ( const char * fmt, ... ); // gcc 4.3 : needed to add const
  

  void SetInteractive(){sType=kINTER;}
  void SetBatch(){sType=kBATCH;}
  unsigned short DoFCS();
  unsigned short DoFCS(int size);
  unsigned short DoFCS(unsigned char *data, int size);
  unsigned short DoFCS(unsigned short *data, int size);

  void SetDEBUG(int mode);
  int CommandEPP(unsigned int addrl, unsigned char cmd, int args, ... );
  int CommandEPP(int args, unsigned short *params, int mode24=0);
  void CommandPCI(unsigned int addrl, unsigned char cmd, int args, ... );
  void CommandPCI(int args, unsigned short *params, int mode24=0);
  void Command(unsigned int addrl, unsigned char cmd, int args, ... );
  void Command2(unsigned int addrl, unsigned char cmd, int args, ushort* params );
  void Command(int args, unsigned short *params, int mode24=0);
  unsigned short SaveJLV1Conf(unsigned int jj);
  unsigned short WriteJLV1Conf(unsigned int jj);

  int ProcessHexCommand(unsigned int addr,ushort cmd,int nparam, ushort * param);
  //int CommandPCI(char *command, unsigned short addr, unsigned char cmd, int args, unsigned short *params);
  // * calibration will fill in this fields
  double ped[1024];
  double sig[1024];
  double sigr[1024];
  unsigned int sta[1024];
  
  unsigned short Event[EVENTSIZE+1];
  //  unsigned short Datain[EVENTSIZE+1];

  CalibMemory CalParMem;
  
  bool Init();
  bool Shut();
  // * Enable or Disable Epp connection (static)
  static bool InitEpp ( );
  static bool ShutEpp ( );
  
        
  AMSWcom ( int port=0 , int hardware=kAMSW_EPP, int card=0);
  ~AMSWcom( );
  //-------------------------------------------------
  //Printout functions
  void PrintEvent();
  void PrintRXDONE();
  void ShowConnect(SlaveMask mask);
  void ShowTDRs(unsigned int mask);
  void PrintSummary( unsigned int address);
  void PrintBuildStat(unsigned short e1);
  void PrintNodeStatus( unsigned int addr);
  //-------------------------------------------------

  void SetHW(int hardware); // set the hardware type
  void SetPort(int portnum); // defines the port to be used  
  void SetPCICard(int card); // defines the card id
  int GetHW() { return hwtype; }
  int GetPort() { return port; }

        // * Let's go with commands
  void Boot(unsigned int addr, unsigned short fname=0);
  //-------------------------------------------------
  //Flash commands
  void FlashLoad(unsigned int addr, unsigned short fname);
  void FlashErase(unsigned int addr, unsigned short fname);
  void FlashRead(unsigned int addr, unsigned short fname,char* fout=0);
  void FlashDefault(unsigned int addr, unsigned short fname, unsigned short def);
  int  CreateFlashFile(unsigned int amswaddr);
  int  WriteFlashFile(unsigned int amswaddr, const char *name);
  void FlashWrite(unsigned int addrl, unsigned short length);
private:
  int  SaveFlashFile( char *name);
  //-------------------------------------------------
public:
  void WriteDelay(unsigned int addr, unsigned short delay);
  unsigned short ReadDelay(unsigned int addr);
  void GetSummary (unsigned int addr);
  //  void Command_ReadMemory          ( unsigned short addr, bool progData, unsigned short address, unsigned short length );
  void ReadDM(unsigned int addr, unsigned short start, unsigned short length);
  void ReadPM(unsigned int addr, unsigned short start, unsigned short length, int mode24);
  void ReadMemory(unsigned int addr, unsigned short start, unsigned short length, int memtype, int mode24);
  void WriteDM(unsigned int addr, unsigned short start, unsigned short length);
  void WritePM(unsigned int addr, unsigned short start, unsigned short length);
  void PerformIO(unsigned int addr, unsigned short seglen, unsigned short segaddr);
  void Ping(unsigned int addr, int nval);
  void Ping(unsigned int addr, unsigned short length);
  void Ping(ushort addr);

  void WriteMemory(unsigned int addr, unsigned short start, unsigned short length, int memtype);
  void Calibrate(unsigned int addr);
  void Calibrate(unsigned int addr, unsigned short par);
  void Calibrate(unsigned int addr, unsigned short par, unsigned short par2);
  void CalibRead(unsigned int addr, unsigned short mode);
  void CalibrateDac(unsigned int addr, unsigned short dac);
  void SaveCalibration(unsigned int addr,char * fname=0);

 ushort  GetCalibration(unsigned int addr,ushort par);
  void SetMode(unsigned int addr, unsigned short mode);
  unsigned short ReadMode(unsigned int addr);
  SlaveMask ReadSlaveMask(unsigned int addr);
  void WriteSlaveMask(unsigned int addr, unsigned short id, unsigned int mask);
  void WriteSlaveMask(unsigned int addr, unsigned short val1, unsigned short val2);
  void WriteBusyMask(unsigned int addr, unsigned int mask);
  void WriteBusyMask(unsigned int addr, unsigned short val1, unsigned short val2);
  unsigned int ReadBusyMask(unsigned int addr);
  unsigned int ReadBusyStat(unsigned int addr);
  void LecroyWrite(ushort addr, ushort bus);
  uint LecroyRead(unsigned int addr, ushort bus, ushort lrcommand, ushort parameter);
  ushort CheckLecroyReply(ushort bus, ushort lrcommand, ushort parameter, int &ok);
  //  void LecroyRead(ushort addr, ushort ncmd, ushort *delay, ushort *bus, ushort *lrcommand, ushort *parameter);
  void LecroyRead(unsigned int addrl, QList *qlist);
  void SlaveTest(unsigned int addr);
  void SetSSF(unsigned int addr, unsigned short setting);
  unsigned short GetSSF(unsigned int addr);
  void SDprocRead(unsigned int addr, unsigned short mode);
  unsigned short GetEvent(unsigned int addr);
  unsigned short GetLastEventN(unsigned int addr);
  void EventReset(unsigned int addr);
  void GetStatus(unsigned int addr);

  void WriteQList(unsigned int addr, QList *qlist);
  void ReadQList(unsigned int addr);

  void SetDataSize(unsigned short size) { DataSize=size; }
  void Command_Lv1(int);
  int Command_Lv1();
  
  int GetEventSize() { return EventSize; }
  int Convert24();

  void SetTimeOut(int timems) { TimeOut=timems; }
  int GetTimeOut() { return TimeOut; }  

  unsigned short GetRX_DONE() { return  RX_DONE; }
  FlashList GetSummary(int sector) { return FlashSummary[sector]; }
  unsigned short GetBuildStat() { return BuildStat; }
  unsigned short GetCRC() { return CRC; }
  NodeStatus GetNodeStatus() { return NStat; }

  void SetCalParMem(CalibMemory *calmem);
  //  void Lecroy(unsigned short addr, unsigned short lrcommand, unsigned short parameter);

};


// * epp amsw codes follow:

/***************************************************************************
** Headers for AMSW-communication program                                  *
**                                                                         *
** Created:  Tue Oct 8, 2002  - A.Kunin                                    *
****************************************************************************/


/* EPP port definitions                                                   */

#define EPPBASE     0x378
#define EPPSTATUS   EPPBASE+1
#define EPPCONTROL  EPPBASE+2
#define EPPADDRESS  EPPBASE+3
#define EPPDATA     EPPBASE+4
#define ECRCONTROL  EPPBASE+0x402

#define S0       0           /* EPP timeout              (not a pin)               */
#define S3       3           /* spare at the moment                                */
#define S4       4           /* spare at the moment                                */
#define S5       5           /* spare at the moment                                */
#define S6       6           /* IRQ                                                */
#define S7       7           /* handshaking: low - start EPP, high - end EPP cycle */

#define C0       0           /* handshaking: low - Write, high -Read               */
#define C1       1           /* handshaking: low when DATA transfer                */
#define C2       2           /* reset EPP: assert then deassert                    */
#define C3       3           /* handshaking: low when ADDRESS transfer             */
#define C4       4           /* enables IRQ               (not a pin)              */



/* AMSWIRE definitions                                                     */

// * MASTER RX/TX buffers (n = port (0123))
#define  cBM_PORT_RXBUF(n)  (0x0000 | ((n)<<14) ) // * (0x0000 - 0x4000 - 0x8000 - 0xC000)
#define  cBM_PORT_TXBUF(n)  (0x1000 | ((n)<<14) ) // * (0x1000 - 0x5000 - 0x9000 - 0xD000)


#define  cBM_PORT0_RXBUF    0x0000     // MASTER RX/TX buffers
#define  cBM_PORT0_TXBUF    0x1000
#define  cBM_PORT1_RXBUF    0x4000
#define  cBM_PORT1_TXBUF    0x5000
#define  cBM_PORT2_RXBUF    0x8000
#define  cBM_PORT2_TXBUF    0x9000
#define  cBM_PORT3_RXBUF    0xC000
#define  cBM_PORT3_TXBUF    0xD000

#define  cPORT_TXBIT_RUN    0x8000     // 1, if TX is running
#define  cPORT_TXBIT_BC2    0x2000     // 1, if end-of-block
#define  cPORT_TXBIT_BC1    0x1000     // 1, if begin-of-block

#define  cPORT_RXBIT_RUN    0x8000     // 1, if RX is running
#define  cPORT_RXBIT_DONE   0x4000     // 1, if RX is done
#define  cPORT_RXBIT_OVRN   0x2000     // 1, if RX overflow
#define  cPORT_RXBIT_MFL    0x1000     // 1, if memory failure
#define  cPORT_RXBIT_BOT    0x0800     // 1, if boot command
#define  cPORT_RXBIT_PER    0x0400     // 1, if parity error
#define  cPORT_RXBIT_MOFL   0x0200     // 1, if memory overflow
#define  cPORT_RXBIT_BC2    0x0100     // 1, if end-of-block
#define  cPORT_RXBIT_BC1    0x0080     // 1, if begin-of-block
#define  cPORT_RXBIT_FER    0x0040     // 1, if format error
#define  cPORT_RXBIT_FLD    0x003F     // error field

#define  cPORT_RXBIT_ERROR  0xB640
#define  cPORT_RXBIT_OVLAP  0xD000

#define psize   4095

// * n = port (0123)
#define  ioPORT_TXAD(n)        ( 0 + ((n)*4))
#define  ioPORT_TXST(n)        ( 1 + ((n)*4))
#define  ioPORT_RXAD(n)        ( 2 + ((n)*4))
#define  ioPORT_RXST(n)        ( 3 + ((n)*4))

#define  ioPORT0_TXAD       0          // RW
#define  ioPORT0_TXST       1          // RW
#define  ioPORT0_RXAD       2          // RW
#define  ioPORT0_RXST       3          // RO
#define  ioPORT1_TXAD       4          // RW
#define  ioPORT1_TXST       5          // RW
#define  ioPORT1_RXAD       6          // RW
#define  ioPORT1_RXST       7          // RO
#define  ioPORT2_TXAD       8          // RW
#define  ioPORT2_TXST       9          // RW
#define  ioPORT2_RXAD       10         // RW
#define  ioPORT2_RXST       11         // RO
#define  ioPORT3_TXAD       12         // RW
#define  ioPORT3_TXST       13         // RW
#define  ioPORT3_RXAD       14         // RW
#define  ioPORT3_RXST       15         // RO

#define  ioBM_ADDR_WR       16         // RW
#define  ioBM_ADDR_RD       17         // RW
#define  ioBM_DATA_WR       18         // WO
#define  ioBM_DATA_RD       19         // RO
#define  ioIRQ_MASK         20         // RW
#define  ioIRQ_RREG         21         // RO

#define  ioBM_CRC_WR        22         // RW
#define  ioBM_CRC_RD        23         // RW

#define  ioLV1_START        24         // W
#define  ioBUSY_STAT        24         // R


/* AMSWIRE Commands                                                     */

#define  cAMSW_BOOT         0x0040     // reboot

#define  cAMSW_EVNTGET      0x0001     // get one event
#define  cAMSW_EVNTNUM      0x0002     // get last event number
#define  cAMSW_EVNTRST      0x0042     // reset FIFOs
#define  cAMSW_HKINFO       0x0003     // read HK info

#define  cAMSW_FLASHRD      0x0005     // Read a file from FLASH
#define  cAMSW_FLASHWR      0x0045     // Write a file to FLASH
#define  cAMSW_FLASHTST     0x0006     // Test FLASH file
#define  cAMSW_FLASHLD      0x0046     // Load FLASH file
#define  cAMSW_FLASHDIR     0x0007     // FLASH summary
#define  cAMSW_FLASHERASE   0x0047     // FILE/SECTOR erase
#define  cAMSW_FLASHDF      0x0048     // change FLASH file attribute

#define  cAMSW_GETSTAT      0x000C     // get node status
#define  cAMSW_PING         0x000D     // ping
#define  cAMSW_PWRDOWN      0x004E     // Enter POWER DOWN

#define  cAMSW_IO           0x000F     // perform IO
#define  cAMSW_READPM       0x0010     // read  PM
#define  cAMSW_WRITEPM      0x0050     // write PM
#define  cAMSW_READDM       0x0011     // read  DM 
#define  cAMSW_WRITEDM      0x0051     // write DM

#define  cAMSW_DELAYRD      0x0019     // read delay
#define  cAMSW_DELAYWR      0x0059     // write delay

#define  cAMSW_RDMODE       0x0012     // read event processing mode
#define  cAMSW_SDPROC_RD    0x0014     // SD proc results reading
#define  cAMSW_CHMODE       0x0052     // change event processing mode
#define  cAMSW_CALPEDRD     0x0013     // ped Calibration read
#define  cAMSW_CALPED       0x0053     // perform Calibration
#define  cAMSW_CALDAC       0x0054     // perform DAC Calibration
#define  cAMSW_CALRED       0x0013     // Read Calibration results

#define  cAMSW_PROGTST      0x0055     // Program test
#define  cAMSW_SLAVTST      0x0056     // SLAVE test

#define  cAMSW_MASKRD       0x0017     // read mask
#define  cAMSW_MASKLD       0x0057     // load mask

#define  cAMSW_BUSYRD       0x0018     // read busy mask
#define  cAMSW_BUSYLD       0x0058     // load busy mask

#define  cAMSW_BUSYSTAT     0x000A     // read busy status

#define  cAMSW_LRSRD        0x001D     // Read  LRS
#define  cAMSW_LRSWR        0x005D     // Write LRS

#define  cAMSW_SSFWR        0x005A     // Set SSF
#define  cAMSW_SSFRD        0x001A     // Read SSF status

#define cAMSW_QLISTWR       0x005C     // write QList
#define cAMSW_QLISTRD       0x001C     // read QList
#define cAMSW_QLISTER       0x005B     // erase QList

#define  cAMSW_RUN          0xFF00     // collect data
#define  cAMSW_PED          0xFF01     // collect pedestals
#define  cAMSW_FILECR       0xFF02     // create file
#define  cAMSW_DAC          0xFF03     // collect DAC
#define  cAMSW_MOTCR        0xFF04     // create MOTOROLA file (ROM Mon)
#define  cAMSW_PINGTEST     0xFF05     // ping test
#define  cAMSW_ECHOTEST     0xFF06     // echo test
#define  cAMSW_TESTBOOT     0xFF08     // test_boot command
#define  cAMSW_TESTLV1      0xFF09     // test_lv1 command
#define  cAMSW_LV1          0xFF0A     // lv1 start
#define  cAMSW_COMMAND      0xFF10     // send command, parameters follow
#define  cAMSW_COMCRC       0xFF11     // protect command by CRC, send it
#define  cAMSW_ANCILLARY    0xFF12     // ancillary command
#define  cAMSW_TEST         0xFF18     // test command
#define  cAMSW_TEST2        0xFF19     // test command
#define  cAMSW_WRITE_PED    0xFF81     // write cal in dsp memory
#define  cAMSW_WRITE_FILE_DM    0xFF82     // write data in memory from file
#define  cAMSW_WRITE_CAL_PAR 0xFF83     // write in DM calibration parameters
#define  cAMSW_GCOMMAND         0xFFFF     // Give a raw command


#endif

