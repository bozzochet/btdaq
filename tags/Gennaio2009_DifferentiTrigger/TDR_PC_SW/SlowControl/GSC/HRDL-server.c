// file HRDL-server.c
//
// This was stolen from Peter Dennett (file HRDLfep.c ao) and reworked
//
// A.Lebedev - Oct-2007...

#include "padlib.h"

//~============================================================================

//  HRDLfep.c   Ground support application for HRDL commanding

//  Revision History
//    July 2004 by Peter Dennett
//      - Initial version at PADSOFT

static char TaskName[] = "HRDLfep";

aalMutex_t ClientInputMutex = aalMutexInitializer;
int ClientInputMask = 0;  // 1 << Client#

//~---  Command line parameters

#define CMD_HELP      'h'
#define CMD_DEBUG     'd'
#define CMD_APID      'a'
#define CMD_SYNCS     1000
#define CMD_DATA      1001
#define CMD_PAD       1002
#define CMD_FRAME     1003
#define CMD_LOOPBACK  1004
#define CMD_CHECKWORD 1005
#define CMD_NOTINIT   'n'

const char           short_options[] =  "hd:a:n";
static struct _CmdOpt long_options[] =
{
  {"help",      0, 0, CMD_HELP,     "Show this listing.", NULL},
  {"debug",     1, 0, CMD_DEBUG,    "Bigger is more.", "Debug"},

  {"apid",      1, 0, CMD_APID,     "The APID to listen to on the network.\n"
                                    "AMSHK, AMSSCI, AMSCMD, AMSHKH, PBCKSCK, PBCKHKH, "
//????????????????????????????????   "PTOPCMD, PTOPREPLY, ACOPCMD, ACOPHK, ACOPQC, "
                                    "APLS_C, APLS_D, ACOP_C, ACOP_D, DDRS2_C, DDRS2_D, "
                                    "FILL\n"
                                    "Note: the HRDL traffic uses PTOPCMD and PTOPREPLY",
                                    "APID"},
  {"syncs",     1, 0, CMD_SYNCS,    "Count of sync symbols between data symbols.",
                                    "Syncs"},
  {"data",      1, 0, CMD_DATA,     "Count of data symbols between sync symbols.",
                                    "Data"},
  {"pad",       1, 0, CMD_PAD,      "Count of sync symbols between frames for padding.",
                                    "Pad"},
  {"frame",     1, 0, CMD_FRAME,    "Count of bytes in a frame for HRDL.", "FrameSize"},
  {"loopback",  0, 0, CMD_LOOPBACK, "Request the hardware loopback within the board "
                                    "(does not requre any loopback cable).\n"
                                    "(Note: this toggles the current setting.)", NULL},
  {"checkword", 0, 0, CMD_CHECKWORD,"Use CCSDS check words.\nDefault is TRUE.\n"
                                    "(Note: this toggles the current setting.)", NULL},
  {"notinit",   0, 0, CMD_NOTINIT,  "Don't initialize interfaces at the beginning", NULL},
  {0, 0, 0, 0, 0, 0}
};

int32 Debug = 0;
int16 APID = APID_ACOPCMD;        // X.Cai - Changed, was 0;
int Port = AAL_TCP_HRDL_PORT;
int32 Syncs = 10;
int32 Data  = 1;
int32 FrameSize = 4080;
int32 Pad = 25000;
int32 LoopBack = FALSE;
int32 CheckWord = TRUE;

int32 TxFIFO_A = 64;
int32 RxFIFO_A = 64;
int32 TxFIFO_B = 64;
int32 RxFIFO_B = 64;

FEP Fep;
frmListHead TxFrames;
int8 InputBuffer[4096];
FRAME InputFrame;

// X.Cai - Added (Start)
FRAME HKFrame;
FRAME SciFrame;
FRAME PPCmdFrame;

int opt_M = 0;
int opt_P = 0;
int opt_S = 0;
int opt_R = 0;

int new_line = 0;

unsigned short cnt_cmd = 0;
unsigned short cnt_ppc = 0;
unsigned short cnt_ppr = 0;
unsigned short cnt_hkh = 0;
unsigned short cnt_sci = 0;
unsigned short cnt_unk = 0;
unsigned short cnt_err = 0;
// X.Cai - Added (End)

int32 LineLen = 24;

#define INTERFACE_A         0
#define INTERFACE_B         1

int interface   = INTERFACE_A;
int inf_started = FALSE;
int inf_init    = TRUE;

#define ACOPNODE            22

//  The contents AMSBlocks supported by this task.

static void PollDeviceInput(void);
static void PollDeviceOutput(void);

//~============================================================================

void usage(void) {

  printf("Usage:\n");
  printf("Provides ground support commanding via the HRDL interface.\n");
  printf("\n");
  CmdOptUsage(short_options, long_options);
}

//~----------------------------------------------------------------------------

void Callback(pFEP pFep, int Function, int Client) {

//  This call back is not from the main thread.  All that should be
//  done is to request the main thread take action.

  switch (Function) {

    case fepClientConnect:
      //      printf("%s:  fepCallback: Connect Client[%d]\n", pFep->TaskName, Client);
    break;

    case fepClientDisconnect:
      //      printf("%s:  fepCallback: Disconnect Client[%d]\n", pFep->TaskName, Client);
    break;

    case fepClientRecv:

      if (pFep->Debug > 3)
        printf("%s:  fepCallback: Recv Client[%d]\n", pFep->TaskName, Client);

      aalMutexLock(&ClientInputMutex);
      ClientInputMask |= 1 << Client;
      aalMutexUnlock(&ClientInputMutex);

    break;

    default:
      printf("%s:  fepCallback: Unknown Function %d Client[%d]\n",
             pFep->TaskName, Function, Client);
    break;
  }
}

//~----------------------------------------------------------------------------

//-Static: HRDL_ISR
//-Synopsis:    ISR
//-ARG_I:       ISRStatus       The device status
//-Desciption:  ISR
//-Returns:     TRUE

static int HRDL_ISR(int32 ISRStatus) {

  if (Debug > 4)  printf("HRDL_ISR: 0x%08X\n", ISRStatus);

  //  if (ISRStatus != 0)
  if (inf_started)  {
    PollDeviceInput();
    PollDeviceOutput();
  }

  return 1;
}

//~----------------------------------------------------------------------------

int ACOP6T_Start(void) {

  int32 reg;

  if (inf_started) return 0;

  if (interface == INTERFACE_A) {
    reg = INT_MASK_ALL & ~(INT_TXA_NF_MASK | INT_RXA_NE_MASK);
    ACOP6TWriteReg(ACOP6T_INT_MASK, reg);
    if (Debug > 2) printf("RegisterISR: Mask: 0x%08X\n", reg);
    reg = TXx_OUT_EN | TXx_DATA_EN;
    ACOP6TWriteReg(ACOP6T_TXA_EN, reg);        // Enable TX output
  }

  else {
    reg = INT_MASK_ALL & ~(INT_TXB_NF_MASK | INT_RXB_NE_MASK);
    ACOP6TWriteReg(ACOP6T_INT_MASK, reg);
    if (Debug > 2) printf("RegisterISR: Mask: 0x%08X\n", reg);
    reg = TXx_OUT_EN | TXx_DATA_EN;
    ACOP6TWriteReg(ACOP6T_TXB_EN, reg);        // Enable TX output
  }

  inf_started = TRUE;
  printf("%s: ACOP6T Inetrface %s started\n", TaskName, (interface ? "B" : "A"));

  return 0;
}

//~----------------------------------------------------------------------------

void ACOP6T_Stop(void) {

  if (!inf_started) return;

  inf_started = FALSE;

  ACOP6TWriteReg(ACOP6T_INT_MASK, INT_MASK_ALL);
  if (interface == INTERFACE_A) ACOP6TWriteReg(ACOP6T_TXA_EN, 0); // Disable TX output
  else                          ACOP6TWriteReg(ACOP6T_TXB_EN, 0); // Disable TX output
  printf("%s: ACOP6T Inetrface %s stopped\n", TaskName, (interface ? "B" : "A"));
}

//~----------------------------------------------------------------------------

//-Static: PollDeviceInput
static void PollDeviceInput(void)
//-Synopsis:    Poll the device for input
//-Returns:         N/A
//-Description: Poll the device for input
//-SeeAlso:
//-Includes:
//-Method:
//-Restrictions:
//-Notes:
//-Bugs:
//-Revisions:
//-End:
{
  int16 APID;
  int8 *b;
  pAMSBlock pRequest;
  int j;
  pFEP pFep = &Fep;
  FRAME *Frame;        // X.Cai - Added
  char name[20];
  int8 chd_data[20];

  while(ACOP6TChkRx(interface))
  {
//  Read data from FIFO into buffer.  First 4 bytes are status for ACOP6T

    if ( ! ACOP6TRead(ACOP6TGetRx(interface), InputBuffer, FrameSize + 4))
    {
      PRINT_NEW_LINE_IF_NEEDED;
      printf("%s: ACOP6TRead failed\n", TaskName);
      cnt_err++;
      return;
    }

    b = &InputBuffer[4];
    APID = (((b[0]<<8)+b[1]) & 0x7FF);

// X.Cai - Changed (Start)
    if ( opt_P ) {
      PRINT_NEW_LINE_IF_NEEDED;
      printf("Frame Received, APID=%d\n", APID);
      HexDump(b, FrameSize, TRUE);
      opt_P = 0;
    }

    switch (APID)
    {
      case APID_PTOPREPLY:
        Frame = &InputFrame;
        strcpy(name, "fepSend[PPR]");
        cnt_ppr++;
        break;
      case APID_PTOPCMD:
        Frame = &PPCmdFrame;
        strcpy(name, "fepSend[PPC]");
        cnt_ppc++;
        break;
      case APID_AMSHKH:
        Frame = &HKFrame;
        strcpy(name, "fepSend[HKH]");
        cnt_hkh++;
        break;
      case APID_AMSSCI:
        Frame = &SciFrame;
        strcpy(name, "fepSend[SCI]");
        cnt_sci++;
        break;
      default:
        Frame = NULL;
        cnt_unk++;
        break;
    }

    // CHD data display
    if ( opt_M ) {
      for ( j = 0; j < 10; j += 2 ) {
        chd_data[j] = b[16 + 2 + j + 1];
        chd_data[j + 1] = b[16 + 2 + j];
      }
      display_CHD(0, chd_data);
      new_line++;
    }

    // Science data output option
    if ( APID == APID_AMSSCI && !opt_S ) {
      ACOP6TIncRx(interface);
      continue;
    }

    if (Frame)
    {
      if (Debug > 2) {
        PRINT_NEW_LINE_IF_NEEDED;
        printf("%s: HRDL Frame Received\n", TaskName);
      }

      while (frmDeblock(Frame, b, &pRequest))
      {
        // Block output to clients
        for (j=0; j<((sizeof pFep->Client) / (sizeof pFep->Client[0])); j++)
        {
          if (pFep->Client[j].Accepted)
          {
            if (Debug > 0) {
              PRINT_NEW_LINE_IF_NEEDED;
              abiPrint(name, pRequest, LineLen);
            }
            fepSend(pFep, pRequest, j);
          }
        }
        abiDestruct(pRequest);
      }
    }

// X.Cai - Changed (Start)

    else
    {
      if (Debug > 2) {
        PRINT_NEW_LINE_IF_NEEDED;
        printf("\n%s: Ignore APID %d\n", TaskName, APID);
      }
    }

    ACOP6TIncRx(interface);
  }

  return;
}

//~----------------------------------------------------------------------------

//-Static: PollDeviceOutput
static void PollDeviceOutput(void)
//-Synopsis:    Poll the device for output
//-Returns:         N/A
//-Description: Poll the device for output
//-SeeAlso:
//-Includes:
//-Method:
//-Restrictions:
//-Notes:
//-Bugs:
//-Revisions:
//-End:
{
  pfrmListEnt pFL;

  while((ACOP6TChkTx(interface)) && (pFL=frmListRemove(&TxFrames)))
  {
    if (Debug > 2) {
      PRINT_NEW_LINE_IF_NEEDED;
      printf("%s: Sending frame %d\n", TaskName, FrameSize);
    }

    if ( ! ACOP6TWrite(ACOP6TGetTx(interface), &pFL->Buffer[0], pFL->FrameSize)) {
      PRINT_NEW_LINE_IF_NEEDED;
      printf("%s: ACOP6TWrite failed\n", TaskName);
      cnt_err++;
    }

    ACOP6TIncTx(interface);
    frmListEntDestruct(&TxFrames, pFL);
  }

  return;
}

//~============================================================================

int main(int argc, char *argv[]) {

  extern char *optarg;
  int32 reg;
  bool rtn;
  bool Running;
  int ClientMask;
  int Client;
  pAMSBlock pRequest;
  FRAME Frame;
  time_t tn;

//~---

  printf("%s: 1.0 Compiled: " __DATE__ " " __TIME__ "\n"
         "  Peter Dennett PADSOFT, Inc  pdennett@padsoft.com  281 334 3800\n", TaskName);

  // opterr = 0; // Turn off getopts error handling

  while (1) {
    int option_index = 0;
    int ch;

    ch = CmdOptGet(argc, argv, short_options, long_options, &option_index);
    if (ch == -1) break;

    switch (ch) {
      case CMD_DEBUG:
        Debug = strtol(optarg, NULL, 0);
      break;

      case CMD_APID:
        APID = apidLookupValue(optarg);

        if (APID == 0) {
          fprintf(stderr, "APID %s was not located\n", optarg);
          exit(1);
        }
      break;

      case CMD_SYNCS:
        Syncs = strtol(optarg, NULL, 0);
        if (Syncs > 255) Syncs = 255;
        if (Syncs < 1)   Syncs = 1;
      break;

      case CMD_DATA:
        Data = strtol(optarg, NULL, 0);
        if (Data > 255) Data = 255;
        if (Data < 1)   Data = 1;
      break;

      case CMD_PAD:
        Pad = strtol(optarg, NULL, 0);
        if (Pad < 2500) Pad = 2500;
      break;

      case CMD_FRAME:
        FrameSize = strtol(optarg, NULL, 0);
        if (FrameSize < 100)  FrameSize = 100;
        if (FrameSize > 4092) FrameSize = 4092;
      break;

      case CMD_LOOPBACK:
        LoopBack = ! LoopBack;
      break;

      case CMD_CHECKWORD:
        CheckWord = ! CheckWord;
      break;

      case CMD_NOTINIT:
        inf_init = FALSE;
      break;

      case '?':
      case CMD_HELP:
      default:
        usage();
        exit(0);
      break;
    }
  }

//~---

  frmListHeadConstruct(&TxFrames, TaskName);

  if (!ACOP6TOpen()) {
    printf("%s: No HRDL device detected\n", TaskName);
    exit(1);
  }

  reg = ACOP6T_RxA | ACOP6T_RxB;

  if (LoopBack) reg |= ACOP6T_LoopBackA;

  rtn = ACOP6TConfig(reg, FrameSize,
                     TxFIFO_A, RxFIFO_A,
                     TxFIFO_B, RxFIFO_B,
                     Data, Syncs, Pad,
                     Data, Syncs, Pad);
  if (!rtn) {
    printf("%s: HRDL device failed to configure\n", TaskName);
    exit(1);
  }

  ACOP6TInstallISR(HRDL_ISR);

  if (Debug > 2) ACOP6TShowCondition();

  if ( inf_init ) ACOP6T_Start();

  if (!frmConstruct(&InputFrame, APID_PTOPREPLY, FrameSize, 0)) {
    printf("%s: Error on frmConstruct (PTOPREPLY)\n", TaskName);
    exit(1);
  }

  if (!frmConstruct(&PPCmdFrame, APID_PTOPCMD, FrameSize, 0)) {
    printf("%s: Error on frmConstruct (PTOPCMD)\n", TaskName);
    exit(1);
  }

  if (!frmConstruct(&HKFrame, APID_AMSHKH, FrameSize, 0)) {
    printf("%s: Error on frmConstruct (AMSHKH)\n", TaskName);
    exit(1);
  }

  if (!frmConstruct(&SciFrame, APID_AMSSCI, FrameSize, 0)) {
    printf("%s: Error on frmConstruct (AMSSCI)\n", TaskName);
    exit(1);
  }

  if (!frmConstruct(&Frame, APID_PTOPCMD, FrameSize, FO_CHECKWORD)) {
    printf("%s: Error on frmConstruct. FS %d\n", TaskName, FrameSize);
    exit(1);
  }

  printf("Open port %d, APID: %d\n", Port, APID);

  if (!fepConstruct(&Fep, Port, Callback, TaskName, Debug)) {
    printf("%s: fepConstruct failed\n", TaskName);
    exit(1);
  }

  ShortSleep(0, 5000);

  tn = time(NULL) + 30;

//~---

  for (Running = TRUE; Running;) {

//~--- keyboard key hits...

    if (kbhit()) {
      int symbol = fgetc(stdin);
      switch (symbol) {
        case 'q':
        case 'Q':
          PRINT_NEW_LINE_IF_NEEDED;
          Running = FALSE;
        break;

        case 'a':
          PRINT_NEW_LINE_IF_NEEDED;
          printf("%s: Allocate %d Free %d In use %d\n", TaskName,
                 aalPDData.Allocated, aalPDData.Freed,
                 aalPDData.Allocated - aalPDData.Freed);
        break;

        case 'd':
          if (Debug < 15) Debug++;
        break;

        case 'D':
          if (Debug) Debug--;
        break;

        case '+':
          if (LineLen < 64) LineLen++;
        break;

        case '-':
          if (LineLen > 24) LineLen--;
        break;

        // Display CHD data on/off
        case 'm':
        case 'M':
          opt_M ^= 1;
        break;

        // Dump one frame of data
        case 'p':
        case 'P':
          opt_P = 1;
        break;

        // Fetch Science data on/off
        case 's':
        case 'S':
          opt_S ^= 1;
        break;

        // HRDL signal detection status
        case 'h':
        case 'H':
          ACOP6TReadReg(ACOP6T_RXA_STATUS, &reg);
          PRINT_NEW_LINE_IF_NEEDED;
          printf("RX Status=0x%08x, %s, %s\n", reg,
                ((reg & RXx_HRDL_DEC) ? "Signal detected" : "Signal not detected"),
                ((reg & RXx_HRDL_SYNC) ? "Sync detected" : "Sync not detected"));
        break;

        // Statistics display
        case 'r':
        case 'R':
          opt_R ^= 1;
        break;

        case '0':
          if (inf_started && interface != INTERFACE_A) {
            ACOP6T_Stop();
            interface = INTERFACE_A;
            ACOP6T_Start();
          }
          else {
            interface = INTERFACE_A;
          }
        break;

        case '1':
          if (inf_started && interface != INTERFACE_B) {
            ACOP6T_Stop();
            interface = INTERFACE_B;
            ACOP6T_Start();
          }
          else {
            interface = INTERFACE_B;
          }
        break;

        case 'n':
        case 'N':
          ACOP6T_Start();
        break;

        case 'f':
        case 'F':
          ACOP6T_Stop();
        break;

        default:
        break;
      }
    }

//~---

    aalMutexLock(&ClientInputMutex);
    ClientMask = ClientInputMask;
    ClientInputMask = 0;
    aalMutexUnlock(&ClientInputMutex);

//~---

    for (Client=0; ClientMask!=0; Client++) {
printf("Client = %d, ", Client);
printf("ClientMask = %08X, ", ClientMask);
      if (ClientMask & (1 << Client)) {
        ClientMask &= ~(1 << Client);
printf("ClientMask = %08X: ", ClientMask);

        while (fepRecv(&Fep, &pRequest, Client)) {
          cnt_cmd++;
Debug = 1;
          if (Debug > 0) {
            PRINT_NEW_LINE_IF_NEEDED;
            abiPrint("fepRecv[PPC]", pRequest, LineLen);
          }
Debug = 0;

//~--- commands to the server itself...

          if (pRequest->NodeAddress == ACOPNODE) {
            switch (pRequest->DataType) {

              case 0x1fff00:
                if (inf_started && interface != INTERFACE_A) {
                  ACOP6T_Stop();
                  interface = INTERFACE_A;
                  ACOP6T_Start();
                }
                else {
                  interface = INTERFACE_A;
                }
              break;

              case 0x1fff01:
                if (inf_started && interface != INTERFACE_B) {
                  ACOP6T_Stop();
                  interface = INTERFACE_B;
                  ACOP6T_Start();
                }
                else {
                  interface = INTERFACE_B;
                }
              break;

              case 0x1fff02:
                ACOP6T_Start();
              break;

              case 0x1fff03:
                ACOP6T_Stop();
              break;

              case 0x1fff04:
                if (inf_started && interface == INTERFACE_A) break;
                if (inf_started) ACOP6T_Stop();
                interface = INTERFACE_A;
                ACOP6T_Start();
              break;

              case 0x1fff05:
                if (inf_started && interface == INTERFACE_B) break;
                if (inf_started) ACOP6T_Stop();
                interface = INTERFACE_B;
                ACOP6T_Start();
              break;

              case 0x1fff06:
                opt_M ^= 1;
              break;

              case 0x1fff07:
                opt_R ^= 1;
              break;
            }  // switch (pRequest->DataType)
          }
          else {
            frmListBlock(&TxFrames, &Frame, pRequest);
          }
          abiDestruct(pRequest);     // Once the AMSBlock has been copied to the framed
                                     // we release it
        }  // while (fepRecv(&Fep, &pRequest, Client))
      }
printf("\n");
    }

//~--- Flush buffers

    frmListBlock(&TxFrames, &Frame, NULL);

    if (inf_started) {
      PollDeviceInput();
      PollDeviceOutput();
      if (opt_R && time(NULL) >= tn) {
        PRINT_NEW_LINE_IF_NEEDED;
        printf("Statistics: cmd=%-5d ppc=%-5d ppr=%-5d hkh=%-5d sci=%-5d bad=%-5d err=%-5d\n",
                cnt_cmd, cnt_ppc, cnt_ppr, cnt_hkh, cnt_sci, cnt_unk, cnt_err);
        tn = time(NULL) + 30;
      }
    }

    ShortSleep(0, 500);
  }

//~---

  frmDestruct(&Frame);

  if (!fepDestruct(&Fep)) {
    PRINT_NEW_LINE_IF_NEEDED;
    printf("%s: fepDestruct failed\n", TaskName);
    exit(1);
  }

  ACOP6T_Stop();
  ACOP6TUnInstallISR();
  ACOP6TClose();

  return 0;
}

//~============================================================================
