/*~*~*~*-*~*-*~*-*~*-*~*-*~*-*~*-*~*-*~*-*~*-*~*-*~*-*~*-*~*-*~*-*~*-*~*-*~*~*/
/*                                                                           */
/*                    Copyright 2009 by PADSOFT, Inc.                        */
/*                                                                           */
/*    Developed for use on AMS-2, An International Space Station payload     */
/*                      searching for missing matter                         */
/*                                                                           */
/*                        No rights reserved                                 */
/*                                                                           */
/* Developed by:        Peter Armistead Dennett                              */
/*                            PADSOFT, Inc                                   */
/*                          61 Harbor Lane                                   */
/*                        Kemah, Texas 77565                                 */
/*                          (281) 334 3800                                   */
/*                      http://www.padsoft.com                               */
/*                                                                           */
/*~*~*~*-*~*-*~*-*~*-*~*-*~*-*~*-*~*-*~*-*~*-*~*-*~*-*~*-*~*-*~*-*~*-*~*-*~*~*/

//  EHSpd.c   EHS Protocol Data handling

//  Revision History
//    Aug 2009 by Peter Dennett
//      - Initial version in PADSOFT, Inc
//    Dec 2010 by Peter Dennett
//      - Updates to stale data handling at KSC
//    Apr 2011 by Peter Dennett
//      - Updates during AMS Laptop ops to suppress duplicated data
//    Jul 2011 by Peter Dennett
//      - Added course and fine time to MSID lists
//      - Removed repeated heartbeat and let level above handle this.

#include "aallib.h"
#include "EHSpd.h"
#include "logfile.h"
#include "ssptime.h"
#include "time.h"
#include "frm.h"
#include "chd.h"

#define swap16(v) ((int16)((int8)((v)>>8) + ((v)<<8)))

//  The ISS uses the GPS epoch (Midnight 6-Jan-1980).  This is the number of 
//  seconds later that this epoch begins from the Unix timestamp epoch 
//  (Midnight 1-Jan-1970).

#define ISSEPOCH 315964800

//~----------------------------------------------------------------------------

typedef struct _PDLdefs
{
  char *PCN;
  char *Name;
  char *RangePCN;
  char *DataType;
  char *t;
  char *n;
  int  SampleRate;
  int  Syllable;
  int  StartWord;
  int  StartBit;
  int  Length;
  int  EntryType;
} PDLdefs, *pPDLdefs;

//~----------------------------------------------------------------------------

// Define the MSIDs for AMS-02 Health and Status Data

#define PDL_HK     0
#define PDL_CTIME  1
#define PDL_FTIME  2

// These entries, from Karl and Valeta respectively, dont work.
  //{"AMS9999", "OB876TIME",  "HDR_0876L00_BYTE005",  "TEHS", "T", "N", 1, 1,   1,  0,   7, 1}, 
  //{"AMS9999", "OB876TIME",  "PDL0876_0014",  "SUND", "T", "N", 1, 1,   1,  0,   5, 1}, 


static PDLdefs EHSpdPDLdefs[] = 
{
  {"AMS9999", "OB876CTIME", "PDL00876_0008", "IUND", "T", "N", 1, 1,    1, 0,   4, PDL_CTIME}, 
  {"AMS9998", "OB876FTIME", "PDL00876_0009", "IUND", "T", "N", 1, 1,    1, 0,   1, PDL_FTIME}, 
  {"AMS0000", "AMSBlock00", "UAZA01RT0098U", "SUND", "T", "N", 1, 1,    1, 0,  22, PDL_HK}, 
  {"AMS0017", "AMSBlock01", "UAZA01RT0017U", "SUND", "T", "N", 1, 1,   12, 0, 200, PDL_HK},
  {"AMS0018", "AMSBlock02", "UAZA01RT0018U", "SUND", "T", "N", 1, 1,  112, 0, 200, PDL_HK},
  {"AMS0019", "AMSBlock03", "UAZA01RT0019U", "SUND", "T", "N", 1, 1,  212, 0, 200, PDL_HK},
  {"AMS0020", "AMSBlock04", "UAZA01RT0020U", "SUND", "T", "N", 1, 1,  312, 0, 200, PDL_HK},
  {"AMS0021", "AMSBlock05", "UAZA01RT0021U", "SUND", "T", "N", 1, 1,  412, 0, 200, PDL_HK},
  {"AMS0022", "AMSBlock06", "UAZA01RT0022U", "SUND", "T", "N", 1, 1,  512, 0, 200, PDL_HK},
  {"AMS0023", "AMSBlock07", "UAZA01RT0023U", "SUND", "T", "N", 1, 1,  612, 0, 200, PDL_HK},
  {"AMS0024", "AMSBlock08", "UAZA01RT0024U", "SUND", "T", "N", 1, 1,  712, 0, 200, PDL_HK},
  {"AMS0025", "AMSBlock09", "UAZA01RT0025U", "SUND", "T", "N", 1, 1,  812, 0, 200, PDL_HK},
  {"AMS0026", "AMSBlock10", "UAZA01RT0026U", "SUND", "T", "N", 1, 1,  912, 0, 200, PDL_HK},
  {"AMS0027", "AMSBlock11", "UAZA01RT0027U", "SUND", "T", "N", 1, 1, 1012, 0, 200, PDL_HK},
  {"AMS0028", "AMSBlock12", "UAZA01RT0028U", "SUND", "T", "N", 1, 1, 1112, 0, 200, PDL_HK},
  {"AMS0029", "AMSBlock13", "UAZA01RT0029U", "SUND", "T", "N", 1, 1, 1212, 0, 122, PDL_HK},
};

//~----------------------------------------------------------------------------

// Define the MSIDs for AMS-02 Health and Status Data

static PDLdefs EHSformat7[] = 
{
  {"AMS9999", "OB876CTIME", "PDL00876_0008", "IUND", "T", "N", 1, 1,    1, 0,   4, PDL_CTIME}, 
  {"AMS9998", "OB876FTIME", "PDL00876_0009", "IUND", "T", "N", 1, 1,    1, 0,   1, PDL_FTIME}, 
//{"DDRS0001", "SubSetID",  "UEZE07RT0001U", "IUNS", "T", "N", 1, 1,    1, 0,   2, PDL_HK}, 
//{"DDRS0002", "PEPSrvcID", "UEZE07RT0002U", "IUNS", "T", "N", 1, 1,    2, 0,   2, PDL_HK}, 
//{"DDRS0003", "PEPData",   "UEZE07RT0003U", "IUNS", "T", "N", 1, 1,    3, 0,   2, PDL_HK}, 
  {"DDRS0004", "CW",        "UEZE07RT0004J", "IUNS", "T", "N", 1, 1,    4, 0,   2, PDL_HK}, 
  {"DDRS0005", "CycleCnt",  "UEZE07RT0005U", "IUNS", "T", "N", 1, 1,    5, 0,   2, PDL_HK}, 
  {"DDRS0006", "FS_LINK",   "UEZE07RT0006J", "IUNS", "T", "N", 1, 1,    6, 0,   2, PDL_HK}, 
  {"DDRS0007", "CHD",       "UEZE07RT0007U", "IUNS", "T", "N", 1, 1,    7, 0,   2, PDL_HK}, 
  {"DDRS0008", "AMSBlock",  "UEZE07RT0008U", "SUND", "T", "N", 1, 1,    8, 0, 176, PDL_HK}, 
} ;

//~----------------------------------------------------------------------------

static char *EHSpdModeNameStr[] =
{
  "all",
  "rt",
  "dump1",
  "dump2",
  "dump3",
  "playback1",
  "playback2",
  "playback3",
  "playback4",
  "playback5",
  "playback6",
  "playback7",
  "playback8",
  "playback9",
  "playback10",
  "playback11",
};

static char *EHSpdOpsModeNameStr[] =
{
  "all",
  "flight",
  "test",
  "sim",
  "verify",
  "dev",
  "train",
  "offline"
};

//~----------------------------------------------------------------------------

char *EHSpdModeName(int Mode)
{
static char *err = "*ERR*";

  if ((Mode < 0) || (Mode >= (sizeof EHSpdModeNameStr) / (sizeof EHSpdModeNameStr[0])))
    return err;
  return EHSpdModeNameStr[Mode];
}

//~----------------------------------------------------------------------------

int EHSpdMode(char *Name)
{
int i;

  for (i=0; i<((sizeof EHSpdModeNameStr) / (sizeof EHSpdModeNameStr[0])); i++)
  {
    if (strcmp(EHSpdModeNameStr[i], Name) == 0)
      return i;
  }

  return 0;
}

//~----------------------------------------------------------------------------

char *EHSpdOpsModeName(int Mode)
{
static char *err = "*ERR*";

  if ((Mode < 0) || (Mode >= (sizeof EHSpdOpsModeNameStr) / (sizeof EHSpdOpsModeNameStr[0])))
    return err;
  return EHSpdOpsModeNameStr[Mode];
}

//~----------------------------------------------------------------------------

int EHSpdOpsMode(char *Name)
{
int i;

  for (i=0; i<((sizeof EHSpdOpsModeNameStr) / (sizeof EHSpdOpsModeNameStr[0])); i++)
  {
    if (strcmp(EHSpdOpsModeNameStr[i], Name) == 0)
      return i;
  }

  return 0;
}

//~----------------------------------------------------------------------------

// Debug Any: Show all headers, verify lengths
// Debug >0: Show each MSID status
// Debug >1: Show each MSID contents

void EHSpdLogPacket(int8 *GSE, int32 GSEBytes, bool RIC, int32 Debug)
{
pEHS_HEADER pEHS = (pEHS_HEADER) GSE;
pEHS_SECONDARY_HEADER_GSE pEHSSECgse;
pEHS_SECONDARY_HEADER_CDP pEHSSECcdp;
pEHS_SECONDARY_HEADER_PDSS pEHSSECpdss;
pEHS_SECONDARY_HEADER_UDSM pEHSSECudsm;
pCCSDS_HEADER pCCSDS;
pCCSDS_SECONDARY_HEADER_GSE pCCSDSSECgse;
pCCSDS_SECONDARY_HEADER pCCSDSSECpdss;
pEHS_UDSM_PAYLOAD pUDSM_Payload;
pEHS_SECONDARY_HEADER_CORE pEHSSECcore;

static char *modes[16] =
{
  "00", "Flt", "Grd", "Sim", "Ver", "Dev", "Trn", "07", 
  "08", "09",  "0A",  "0B",  "0C",  "0D",  "0E",  "0F",
} ;
static char *types[16] =
{
  "All Protocols",   "TDM-TLM",      "NASCOM",        "Pseudo",
  "TDS Packet",      "EHS-Test",     "GSE",           "CDP",
  "HSDS DQ",         "CSS",          "PDSS AOS/LOS",  "PDSS Payload CCSDS", 
  "PDSS Core CCSDS", "PDSS BPDU",    "PDSS UDSM",     "PDSS RPSM"
} ;
static char *udsm_types[] =
{
  "Invalid", "LOS", "Scheduled end of data", "Operator"
} ;
char typehex[10];
int8 *cin;
int i;
int8 *packetend;
int32 status, count;
int8 *sampstat;
bool bounds = TRUE;
int errors = 0;
pPDLdefs pPDL;
int32    nPDL;
int format_id;

  if (pEHS->sec_pro_type > (sizeof (types) / sizeof (types[0])))
    sprintf(typehex, "0x%X", pEHS->sec_pro_type);

  LogNormal("~~~~~~~~~~~~~~\nHOSC Packet (%s) ProtoVer:%d ProjId:%d DataMode:%s R/T:%d Inc:%d "
            "SecProtoType:%d \n  Length:%d GMT: %d/%d %2.2d:%2.2d:%2.2d\n",
            (pEHS->sec_pro_type > (sizeof (types) / sizeof (types[0]))) ? typehex :
            types[pEHS->sec_pro_type],
            pEHS->identifier, pEHS->projid, 
            modes[pEHS->data_mode], pEHS->support_mode,
            pEHS->increment, pEHS->sec_pro_type,
            swap16(pEHS->hosc_size),
            pEHS->grt_year+1900, swap16(pEHS->grt_yearday), 
            pEHS->grt_hour, pEHS->grt_minute, pEHS->grt_second);
  LogHexDump(pEHS, sizeof *pEHS, 1);

  switch(pEHS->sec_pro_type)
  {
    default:
      LogNormal("  All Headers:\n");
      LogHexDump(GSE, 32*4, 1);
      break;

    case 0x0C:  // Core Packet (796 bytes)
      pEHSSECcore = (pEHS_SECONDARY_HEADER_CORE) (GSE + sizeof *pEHS);
      pCCSDS =  (pCCSDS_HEADER) (GSE + sizeof *pEHS + sizeof *pEHSSECcore);

      LogNormal("  Core Secondary Header: APID %d\n",
                ((pEHSSECcore->DSIDapidMSB)<<8) + pEHSSECcore->DSIDapidLSB);
      LogHexDump(pEHSSECcore, sizeof *pEHSSECcore, 1);

      LogNormal("  CCSDS Primary: APID:%d Sequence:%d Length:%d\n",
                (pCCSDS->apid_msb<<8) + pCCSDS->apid_lsb,
                (pCCSDS->pkt_seq_count_msb<<8) + pCCSDS->pkt_seq_count_lsb,
                (pCCSDS->packet_length_msb<<8) + pCCSDS->packet_length_lsb + 7);
      LogHexDump(pCCSDS, sizeof *pCCSDS, 1);

      LogNormal("  Core Packet:\n");
      LogHexDump(GSE, swap16(pEHS->hosc_size), 1);

      //pEHSSECpdss =(pEHS_SECONDARY_HEADER_PDSS) (GSE + sizeof *pEHS);
      //pCCSDS =  (pCCSDS_HEADER) (GSE + sizeof *pEHS + sizeof *pEHSSECpdss);
      break;

    case 0x0B:
      pEHSSECpdss =(pEHS_SECONDARY_HEADER_PDSS) (GSE + sizeof *pEHS);
      pCCSDS =  (pCCSDS_HEADER) (GSE + sizeof *pEHS + sizeof *pEHSSECpdss);
      pCCSDSSECpdss = (pCCSDS_SECONDARY_HEADER) 
                      (GSE + sizeof *pEHS + sizeof *pEHSSECpdss + sizeof *pCCSDS);

      LogNormal("  PDSS Secondary Header: APID %d PDSS Key: %04X\n",
                ((pEHSSECpdss->DSIDapidMSB)<<8) + pEHSSECpdss->DSIDapidLSB,
                ((pEHSSECpdss->PDSSkeyMSB)<<8) + pEHSSECpdss->PDSSkeyLSB);
      LogHexDump(pEHSSECpdss, sizeof *pEHSSECpdss, 1);

      LogNormal("  CCSDS Primary: APID:%d Sequence:%d Length:%d\n",
                (pCCSDS->apid_msb<<8) + pCCSDS->apid_lsb,
                (pCCSDS->pkt_seq_count_msb<<8) + pCCSDS->pkt_seq_count_lsb,
                (pCCSDS->packet_length_msb<<8) + pCCSDS->packet_length_lsb + 7);
      LogHexDump(pCCSDS, sizeof *pCCSDS, 1);

      LogNormal("  CCSDS Secondary PDSS Header:\n");
      LogHexDump(pCCSDSSECpdss, sizeof *pCCSDSSECpdss, 1);
      break;

    case 0x06:
    case 0x07:
      if (pEHS->sec_pro_type == 0x07)    // CDP
      {
        pEHSSECcdp = (pEHS_SECONDARY_HEADER_CDP) (GSE + sizeof *pEHS);
        pCCSDS =  (pCCSDS_HEADER) (GSE + sizeof *pEHS + sizeof *pEHSSECcdp);
        pCCSDSSECgse = (pCCSDS_SECONDARY_HEADER_GSE) (GSE + sizeof *pEHS +
                                                      sizeof *pEHSSECcdp + sizeof *pCCSDS);
        LogNormal("  CDP EHS Secondary: Workstation %d UserId %d ProcessId %d\n",
                  swap16(pEHSSECcdp->WorkstationID), swap16(pEHSSECcdp->UserID),
                  swap16(pEHSSECcdp->ProcessID));
        LogHexDump(pEHSSECcdp, sizeof *pEHSSECcdp, 1);

        format_id = RIC?7:3;
        cin =  GSE + sizeof *pEHS + sizeof *pEHSSECcdp + sizeof *pCCSDS;
        packetend = cin + ((pCCSDS->packet_length_msb<<8) + pCCSDS->packet_length_lsb) + 1;
      }
      else                               // GSE
      {
        pEHSSECgse = (pEHS_SECONDARY_HEADER_GSE) (GSE + sizeof *pEHS);
        pCCSDS =  (pCCSDS_HEADER) (GSE + sizeof *pEHS + sizeof *pEHSSECgse);
        pCCSDSSECgse = (pCCSDS_SECONDARY_HEADER_GSE) (GSE + sizeof *pEHS +
                                                      sizeof *pEHSSECgse + sizeof *pCCSDS);
        format_id = pCCSDSSECgse->format_id;

        LogNormal("  GSE EHS Secondary: ReqType:%s Routing:%s %s %s APID: %d Format %d\n", 
                  pEHSSECgse->RequestType  ? "User"    : "POIC", 
                  pEHSSECgse->RoutingFlag  ? "PDSS"    : "TNS",
                  pEHSSECgse->DSID         ? "BPDU"    : "CCSDS",
                  pEHSSECgse->DSID_Payload ? "Payload" : "Core",
                  (pEHSSECgse->DSID_APID_MSB<<8) + pEHSSECgse->DSID_APID_LSB,
                  format_id);
        LogHexDump(pEHSSECgse, sizeof *pEHSSECgse, 1);

        cin =  GSE + sizeof *pEHS + sizeof *pEHSSECgse + sizeof *pCCSDS + sizeof *pCCSDSSECgse;
        packetend = cin + ((pCCSDS->packet_length_msb<<8) + pCCSDS->packet_length_lsb) + 1;
      }

      LogNormal("  CCSDS Primary: APID:%d Sequence:%d Length:%d\n",
                (pCCSDS->apid_msb<<8) + pCCSDS->apid_lsb,
                (pCCSDS->pkt_seq_count_msb<<8) + pCCSDS->pkt_seq_count_lsb,
                (pCCSDS->packet_length_msb<<8) + pCCSDS->packet_length_lsb + 7);
      LogHexDump(pCCSDS, sizeof *pCCSDS, 1);

      LogNormal("  CCSDS GSE Secondary: Format:%d Select-Merge:%s Time:%s\n",
                pCCSDSSECgse->format_id, pCCSDSSECgse->select_merge?"On":"Off",
                pCCSDSSECgse->embedded_time?"Embedded":"Generation");
      LogHexDump(pCCSDSSECgse, sizeof *pCCSDSSECgse, 1);

      switch (format_id)
      {
        case 7:
          pPDL = EHSformat7;
          nPDL = (sizeof EHSformat7) / (sizeof EHSformat7[0]);
          break;

        case 3:
        default:
          pPDL = EHSpdPDLdefs;
          nPDL = (sizeof EHSpdPDLdefs) / (sizeof EHSpdPDLdefs[0]);
          break;
      }

      for (i=0; i<nPDL; i++)
      {
        status = 0;
        status += (*cin++)<<24;
        status += (*cin++)<<16;
        status += (*cin++)<< 8;
        status += (*cin++);

        count = 0;
        count += (*cin++)<<24;
        count += (*cin++)<<16;
        count += (*cin++)<< 8;
        count += (*cin++);

        sampstat = cin++;
        bounds = bounds && (cin+pPDL[i].Length) <= packetend;
        if (Debug > 0)
          LogNormal("  %-12.12sCount:%d Length:%3d Status 0x%X Sample Status:'%c'%s\n",
                    pPDL[i].Name, count, pPDL[i].Length, status, *sampstat,
                    bounds ? "" : " Sample goes past end");
        if ((Debug > 1) && bounds)
          LogHexDump(cin, pPDL[i].Length, TRUE);
        cin += pPDL[i].Length;
        if ((count != 1) || (! bounds))
          errors++;
      }
      break;

    case 0x0E:
      pEHSSECudsm =(pEHS_SECONDARY_HEADER_UDSM) (GSE + sizeof *pEHS);
      LogNormal("  UDSM Secondary: ReqType:%s %s APID: %d\n", 
                pEHSSECudsm->ccsds_bpdu  ? "BPDU"    : "CCSDS", 
                pEHSSECudsm->DSIDcore ? "Payload" : "Core",
                (pEHSSECudsm->DSIDapidMSB<<8) + pEHSSECudsm->DSIDapidLSB);
      LogHexDump(pEHSSECudsm, sizeof *pEHSSECudsm, 1);

      pCCSDS =  (pCCSDS_HEADER) (GSE + sizeof *pEHS + sizeof *pEHSSECudsm);
      LogNormal("  CCSDS Primary: APID:%d Sequence:%d Length:%d\n",
                (pCCSDS->apid_msb<<8) + pCCSDS->apid_lsb,
                (pCCSDS->pkt_seq_count_msb<<8) + pCCSDS->pkt_seq_count_lsb,
                (pCCSDS->packet_length_msb<<8) + pCCSDS->packet_length_lsb + 7);
      LogHexDump(pCCSDS, sizeof *pCCSDS, 1);

      pUDSM_Payload = (pEHS_UDSM_PAYLOAD) (GSE + sizeof *pEHS + sizeof *pEHSSECudsm +
                      sizeof *pCCSDS);
      LogNormal("  UDSM Payload: %s %s APID:%d Start GMT: %d/%d %2.2d:%2.2d:%2.2d"
                " End GMT: %d/%d %2.2d:%2.2d:%2.2d\n",
                pUDSM_Payload->ccsds_bpdu ? "BPDU" : "CCSDS", 
                pUDSM_Payload->DSIDcore ? "Payload" : "Core",
                (pUDSM_Payload->DSIDapidMSB<<8) + pUDSM_Payload->DSIDapidLSB, 
                pUDSM_Payload->grt_s_year+1900, swap16(pUDSM_Payload->grt_s_yearday), 
                pUDSM_Payload->grt_s_hour, pUDSM_Payload->grt_s_minute,
                pUDSM_Payload->grt_s_second,
                pUDSM_Payload->grt_e_year+1900, swap16(pUDSM_Payload->grt_e_yearday), 
                pUDSM_Payload->grt_e_hour, pUDSM_Payload->grt_e_minute,
                pUDSM_Payload->grt_e_second);
      LogNormal("            UDSM event: %6d %s\n", pUDSM_Payload->UDSM_event, 
          (pUDSM_Payload->UDSM_event <= 0 || pUDSM_Payload->UDSM_event >= 
           ((sizeof udsm_types)/sizeof udsm_types[0])) ? "Invalid" : 
          udsm_types[pUDSM_Payload->UDSM_event]);
      LogNormal("         Valid packets: %6d\n", swap16(pUDSM_Payload->CntValidPackets));
      LogNormal("      Packets rollover: %6d\n", pUDSM_Payload->CntValidPktRollOvr);
      LogNormal("    VCDU sequence errs: %6d\n", swap16(pUDSM_Payload->CntVCDUSeqErrs));
      LogNormal("  Packet sequence errs: %6d\n", swap16(pUDSM_Payload->CntPktSeqErrs));
      LogNormal("    Packet length errs: %6d\n", swap16(pUDSM_Payload->CntPktLenErrs));
      LogHexDump(pUDSM_Payload, sizeof *pUDSM_Payload, 1);
      break;
  }

  if (errors)
  {
    LogError("EHSpdLogPacket: Data errors %d.\n", errors);
  }
}

//~----------------------------------------------------------------------------

void EHSpdLogCDPPacket(int8 *GSE, int32 GSEBytes, bool RIC, int32 Debug)
{
pEHS_HEADER pEHS = (pEHS_HEADER) GSE;
pEHS_SECONDARY_HEADER_CDP pEHSSECcdp;
pCCSDS_HEADER pCCSDS;

static char *modes[16] =
{
  "00", "Flt", "Grd", "Sim", "Ver", "Dev", "Trn", "07", 
  "08", "09",  "0A",  "0B",  "0C",  "0D",  "0E",  "0F",
} ;

int8 *cin;
int i;
int8 *packetend;
int32 status, count;
int errors = 0;
pPDLdefs pPDL;
int32    nPDL;
int format_id;
int Stale;

  if (pEHS->sec_pro_type != 7)
  {
    LogError("EHSpdLogCDPPacket: Packet not type not CDP\n");
    EHSpdLogPacket(GSE, GSEBytes, RIC, Debug);
    return;
  }

  pCCSDS = (pCCSDS_HEADER) (GSE + sizeof *pEHS + sizeof *pEHSSECcdp);

  format_id = RIC?7:3;
  cin =  GSE + sizeof *pEHS + sizeof *pEHSSECcdp + sizeof *pCCSDS;
  packetend = cin + ((pCCSDS->packet_length_msb<<8) + pCCSDS->packet_length_lsb) + 1;

  switch (format_id)
  {
    case 7:
      pPDL = EHSformat7;
      nPDL = (sizeof EHSformat7) / (sizeof EHSformat7[0]);
      break;

    case 3:
    default:
      pPDL = EHSpdPDLdefs;
      nPDL = (sizeof EHSpdPDLdefs) / (sizeof EHSpdPDLdefs[0]);
      break;
  }

  Stale = 0;
  errors = 0;

  for (i=0; i<nPDL; i++)
  {
    status  = (*cin++)<<24;
    status += (*cin++)<<16;
    status += (*cin++)<< 8;
    status += (*cin++);

    count  = (*cin++)<<24;
    count += (*cin++)<<16;
    count += (*cin++)<< 8;
    count += (*cin++);

  //  The ISS HK data typically spans multiple APID 876 packets.  We get one CDP for 
  //  each occurance of an APID 876 packet.  I consider this a bug, HOSC considers it a
  //  feature.  Therefore if any data is stale we reject the entire packet.

    if (((status & 0x40000000) != 0) || (*cin != ' '))
      Stale++;

    if (((cin + pPDL[i].Length) >= packetend) || (count != 1))
      errors++;

    cin++;
    cin += pPDL[i].Length;
  }

  LogNormal("CDP ProtoVer:%d ProjId:%d DataMode:%s R/T:%d Inc:%d "
            "SecProtoType:%d Length:%d GMT: %d/%d %2.2d:%2.2d:%2.2d Stale %d Errors %d\n",
            pEHS->identifier, pEHS->projid, 
            modes[pEHS->data_mode], pEHS->support_mode,
            pEHS->increment, pEHS->sec_pro_type,
            swap16(pEHS->hosc_size),
            pEHS->grt_year+1900, swap16(pEHS->grt_yearday), 
            pEHS->grt_hour, pEHS->grt_minute, pEHS->grt_second, Stale, errors);

  return;
}

//~----------------------------------------------------------------------------

int EHSpdFrameToGSE(int8 *Frame, int32 FrameBytes, int8 *GSE, int32 *GSEBytes)
{
pEHS_HEADER pEHS = (pEHS_HEADER) GSE;
pEHS_SECONDARY_HEADER_GSE pEHSSEC;
pCCSDS_HEADER pCCSDS;
pCCSDS_SECONDARY_HEADER_GSE pCCSDSSECgse;
int i;
int cnt, cnt2, cnt3, cnt4;
static int seqno = 0;
static int APID = 0x7FF;  //?? Not sure of the impact of using fill, was 975
int8 *cin, *cout;
sspTIMEVAL time_now;
struct tm gmt;
pPDLdefs pPDL;
int32    nPDL;
int32 format_id;

int32 Time;
int32 Fine;

  pEHSSEC = (pEHS_SECONDARY_HEADER_GSE) (GSE + sizeof *pEHS);
  pCCSDS = (pCCSDS_HEADER) (GSE + sizeof *pEHS + sizeof *pEHSSEC);
  pCCSDSSECgse = (pCCSDS_SECONDARY_HEADER_GSE) (GSE + sizeof *pEHS +
                                                sizeof *pEHSSEC + sizeof *pCCSDS);

  if (FrameBytes < 2560)
    format_id = 7;
  else
    format_id = 3;

  switch (format_id)
  {
    case 7:
      pPDL = EHSformat7;
      nPDL = (sizeof EHSformat7) / (sizeof EHSformat7[0]);
      break;

    case 3:
    default:
      pPDL = EHSpdPDLdefs;
      nPDL = (sizeof EHSpdPDLdefs) / (sizeof EHSpdPDLdefs[0]);
      break;
  }

  cnt2 = 0;
  cnt3 = 0;
  cnt4 = 0;

  for (i=0; i<nPDL; i++)
  {
    cnt2 += 4 + 4 + 1;
    if (pPDL[i].EntryType == PDL_HK)
      cnt3 += pPDL[i].Length;
    else
      cnt4 += pPDL[i].Length;
  }

  cnt =  sizeof *pEHS + sizeof *pEHSSEC + sizeof *pCCSDS + sizeof *pCCSDSSECgse +
         cnt2 + cnt3 + cnt4;

  if (cnt >= *GSEBytes)
  {
    LogError("EHSpdFrameToGSE: Format %d buffer too short %d needs %d\n", 
             format_id, *GSEBytes, cnt);
    return 0;
  }

  if ((cnt3 + 6 + 10) != FrameBytes)
  {
    LogError("EHSpdFrameToGSE: Format %d frame size %d does not match PDL definition %d\n",
             format_id, FrameBytes, (cnt3+6+10));
    return 0;
  }

  timeGet(&time_now);
  gmt = *gmtime((time_t *)&time_now.tv_sec);

  Time = time_now.tv_sec - ISSEPOCH;
  Fine = time_now.tv_usec / 6400;

  memset(GSE, 0, cnt);

  pEHS->identifier = 2;   // Version
  pEHS->projid = 3;       // ISS
  pEHS->data_mode = 5;    // Development
  pEHS->support_mode = 1; // RT
  pEHS->increment = 19;   // Increment
  pEHS->sec_pro_type = 6; // GSE Packet Data

  pCCSDSSECgse->format_id = format_id;

  pEHS->grt_year = gmt.tm_year;
  pEHS->grt_yearday = swap16(gmt.tm_yday);
  pEHS->grt_hour = gmt.tm_hour;
  pEHS->grt_minute = gmt.tm_min;
  pEHS->grt_second = gmt.tm_sec;

  pEHS->hosc_size = swap16(cnt);

  pEHSSEC->RequestType = 0;  // POIC
  pEHSSEC->RoutingFlag = 1;  // PDSS
  pEHSSEC->DSID = 0;         // CCSDS
  pEHSSEC->DSID_Payload = 1; // Payload data
  pEHSSEC->DSID_GSE = 1;

  pEHSSEC->DSID_APID_MSB = (int8)(APID>>8);
  pEHSSEC->DSID_APID_LSB = (int8)APID;
  pEHSSEC->UserID = swap16(0x1234);
  pEHSSEC->UserID = swap16(0x5678);

  pCCSDS->apid_msb = (int8)(APID>>8);
  pCCSDS->apid_lsb = (int8)(APID);
  pCCSDS->type = 1;
  pCCSDS->flag = 1;
  pCCSDS->sequence_flag = 3;
  pCCSDS->pkt_seq_count_msb = (int8)(seqno>>8);
  pCCSDS->pkt_seq_count_lsb = (int8)(seqno);
  seqno = (seqno+1) & 0x1FFF;
  pCCSDS->packet_length_msb = (int8)((cnt2+cnt3+cnt4) >> 8);
  pCCSDS->packet_length_lsb = (int8)((cnt2+cnt3+cnt4));

  cin = Frame+16;
  cout =  GSE + sizeof *pEHS + sizeof *pEHSSEC + sizeof *pCCSDS + sizeof *pCCSDSSECgse;

  for (i=0; i<nPDL; i++)
  {
    *cout++ = (int8)(0 >> 24);   // 0 is 'OK' overall status
    *cout++ = (int8)(0 >> 16);
    *cout++ = (int8)(0 >>  8);
    *cout++ = (int8)(0);

    *cout++ = (int8)(1 >> 24);   // 1 Sample
    *cout++ = (int8)(1 >> 16);
    *cout++ = (int8)(1 >>  8);
    *cout++ = (int8)(1);

    *cout++ = (int8)(' ');       // Status OK - AOS

    switch(pPDL[i].EntryType)
    {
      case PDL_CTIME:
        *cout++ = (int8)(Time >> 24);
        *cout++ = (int8)(Time >> 16);
        *cout++ = (int8)(Time >>  8);
        *cout++ = (int8)(Time);
        break;

       case PDL_FTIME:
         *cout++ = (int8)(Fine);
         break;

      default:
      case PDL_HK:
        memcpy(cout, cin, pPDL[i].Length);
        cin += pPDL[i].Length;
        cout += pPDL[i].Length;
        break;
    }
  }

  *GSEBytes = cnt;
  return 1;
}

//~----------------------------------------------------------------------------

int EHSpdGSEtoFrame(int8 *GSE, int32 GSEBytes, int8 *Frame, int32 *FrameBytes)
{
pEHS_HEADER pEHS = (pEHS_HEADER) GSE;
pEHS_SECONDARY_HEADER_GSE pEHSSEC;
pCCSDS_HEADER pCCSDS;
pCCSDS_SECONDARY_HEADER pCCSDSSEC;
pCCSDS_SECONDARY_HEADER_GSE pCCSDSSECgse;
int i;
int cnt;
int8 *cin, *cout;
int32 status, count;
int8 *packetend;
int SeqNo;
//int PktLen;
int APID = 876;
pPDLdefs pPDL;
int32    nPDL;
int32 format_id;
static int FakeSeqNo = 0;
int32 Time = 0;
int32 Fine = 0;

  pEHSSEC = (pEHS_SECONDARY_HEADER_GSE) (GSE + sizeof *pEHS);
  pCCSDS = (pCCSDS_HEADER) (GSE + sizeof *pEHS + sizeof *pEHSSEC);
  pCCSDSSECgse = (pCCSDS_SECONDARY_HEADER_GSE) (GSE + sizeof *pEHS +
                                                sizeof *pEHSSEC + sizeof *pCCSDS);
  format_id = pCCSDSSECgse->format_id;

  switch (format_id)
  {
    case 7:
      pPDL = EHSformat7;
      nPDL = (sizeof EHSformat7) / (sizeof EHSformat7[0]);
      break;

    case 3:
    default:
      pPDL = EHSpdPDLdefs;
      nPDL = (sizeof EHSpdPDLdefs) / (sizeof EHSpdPDLdefs[0]);
      break;
  }

  cnt = 0;
  for (i=0; i<nPDL; i++)
  {
    if (pPDL[i].EntryType == PDL_HK)
      cnt += pPDL[i].Length;
  }

  // Note:  When we get errors we use the first byte of Frame to say if it is
  //        just a "stale" packet. If this first byte is '*' it is a more serious 
  //        offense and there is a message to log following the "stale" indicator.

  if ((cnt+16) > *FrameBytes)
  {
    sprintf((char*)Frame, "*EHSpdGSEtoFrame: Format %d buffer too short %d needs %d",
            format_id, *FrameBytes, cnt);
    return 0;
  }

  //APID   = (pCCSDS->apid_msb<<8) + pCCSDS->apid_lsb;
  SeqNo  = (pCCSDS->pkt_seq_count_msb<<8) + pCCSDS->pkt_seq_count_lsb;
  //PktLen = (pCCSDS->packet_length_msb<<8) + pCCSDS->packet_length_lsb + 7;

  memset(Frame, 0xEE, cnt);

  cout = Frame+16;
  cin =  GSE + sizeof *pEHS + sizeof *pEHSSEC + sizeof *pCCSDS + sizeof *pCCSDSSECgse;
  packetend = cin + ((pCCSDS->packet_length_msb<<8) + pCCSDS->packet_length_lsb) + 1;

  for (i=0; i<nPDL; i++)
  {
    status  = (*cin++)<<24;
    status += (*cin++)<<16;
    status += (*cin++)<< 8;
    status += (*cin++);

    count  = (*cin++)<<24;
    count += (*cin++)<<16;
    count += (*cin++)<< 8;
    count += (*cin++);

    // Any stale MSID spoils the soup

    if (((status & 0x40000000) != 0) || (*cin != ' '))
    {
      if (pPDL[i].EntryType == PDL_HK)
      {
        Frame[0] = ' ';
        Frame[1] = 0;
      }
      else
      {
#if 0
        sprintf((char*)Frame, "*EHSpdGSEtoFrame: Time value %s is '%c'", 
                pPDL[i].Name, *cin);
#else
        Frame[0] = ' ';
        Frame[1] = 0;
#endif
      }
      return 0;
    }

    if (((cin + pPDL[i].Length) >= packetend) || (count != 1))
    {
      sprintf((char*)Frame, "*EHSpdGSEtoFrame: MSID[%i] Fmt %d length %d"
              " or count %d not valid",
              i, format_id, pPDL[i].Length, count);
      return 0;
    }

    cin++;

    switch(pPDL[i].EntryType)
    {
      case PDL_CTIME:
        Time = (*(cin+0) << 24) + (*(cin+1) << 16) + (*(cin+2) <<  8) + (*(cin+3));
        break;

      case PDL_FTIME:
        Fine =  *(cin);
        break;

      default:
      case PDL_HK:
        memcpy(cout, cin, pPDL[i].Length);
        cout += pPDL[i].Length;
        break;
    }
    cin += pPDL[i].Length;
  }

  if (format_id == 3)
    SeqNo = swap16(((int16*)Frame)[12]);  // Get the copy of SeqNo AMS stashed
  else
    SeqNo = FakeSeqNo++;

  memset(Frame, 0, 16);

  pCCSDS = (pCCSDS_HEADER) (Frame);
  pCCSDS->version = 0;
  pCCSDS->type = 1;                     // Payload (not core)
  pCCSDS->flag = 1;                     // Secondary header present
  pCCSDS->apid_msb = (int8)(APID>>8);
  pCCSDS->apid_lsb = (int8)(APID);
  pCCSDS->sequence_flag = 3;
  pCCSDS->pkt_seq_count_msb = (int8)(SeqNo>>8);
  pCCSDS->pkt_seq_count_lsb = (int8)(SeqNo);
  pCCSDS->packet_length_msb = (int8)((cnt+16-7)>>8);
  pCCSDS->packet_length_lsb = (int8)((cnt+16-7));

  pCCSDSSEC = (pCCSDS_SECONDARY_HEADER) ((int8*)Frame + sizeof *pCCSDS);
  cout = (int8*) pCCSDSSEC;
  *cout++ = (int8)(Time >> 24);
  *cout++ = (int8)(Time >> 16);
  *cout++ = (int8)(Time >>  8);
  *cout++ = (int8)(Time);
  pCCSDSSEC->fine_time = (int8) Fine;
  pCCSDSSEC->packet_type = 6;
  pCCSDSSEC->zoe_tlm = 0;
  pCCSDSSEC->checkword_indicator = 0;  // Maybe there but would be invalid
  pCCSDSSEC->time_id = 0;
  pCCSDSSEC->version_id = swap16(0xDEAD);
  pCCSDSSEC->cycle_counter = swap16(0xBEEF);

  //  NOTE:  Since the CCSDS header was stripped we dont know if this
  //         Frame had a CheckWord.  FS_CRC is carried thru but any CRC
  //         value would be useless since it was computed over the CCSDS
  //         header.  Hence all frames coming out of this routine have
  //         CheckWord and FS_CRC cleared.  If computed values for these
  //         exist at the end of the buffer they should lay outside any
  //         AMSBlock segment and ignored by deframing routines.

  //  NOTE:  We have a bug in RICShim that rolls the seq count into FBI
  //         such that we incorrectly de-psuedo randomize.  Since AMS Laptop
  //         1.25 has been released to testing we overcome this here.

  if (format_id == 7)
    Frame[18] &= ~(FS_CRC<<4);  //  If FS_CRC is present turn it off
  else
    Frame[16] &= ~(FS_CRC<<4);  //  If FS_CRC is present turn it off

  *FrameBytes = cnt + 16;
  return 1;
}

//~----------------------------------------------------------------------------

int EHSpdCDPtoFrame(int8 *GSE, int32 GSEBytes, int8 *Frame, int32 *FrameBytes, bool RIC)
{
pEHS_HEADER pEHS = (pEHS_HEADER) GSE;
pEHS_SECONDARY_HEADER_CDP pEHSSEC;
pCCSDS_HEADER pCCSDS;
pCCSDS_SECONDARY_HEADER pCCSDSSEC;
int i;
int cnt;
int8 *cin, *cout;
int32 status = 0;
int32 count;
int8 *packetend;
int SeqNo;
//int PktLen;
int APID = 876;
pPDLdefs pPDL;
int32    nPDL;
int32 format_id;
static int8 HeartBeat = 255;
static int FakeSeqNo = 0;
 
int32 Time = 0;
int32 Fine = 0;

  pEHSSEC = (pEHS_SECONDARY_HEADER_CDP) (GSE + sizeof *pEHS);
  pCCSDS = (pCCSDS_HEADER) (GSE + sizeof *pEHS + sizeof *pEHSSEC);

  format_id = RIC ? 7 : 3;

  switch (format_id)
  {
    case 7:
      pPDL = EHSformat7;
      nPDL = (sizeof EHSformat7) / (sizeof EHSformat7[0]);
      break;

    case 3:
    default:
      pPDL = EHSpdPDLdefs;
      nPDL = (sizeof EHSpdPDLdefs) / (sizeof EHSpdPDLdefs[0]);
      break;
  }

  cnt = 0;
  for (i=0; i<nPDL; i++)
  {
    if (pPDL[i].EntryType == PDL_HK)
      cnt += pPDL[i].Length;
  }

  // Note:  When we get errors we use the first byte of Frame to say if it is
  //        just a "stale" packet. If this first byte is '*' it is a more serious 
  //        offense and there is a message to log following the "stale" indicator.

  if ((cnt+16) > *FrameBytes)
  {
    LogError("EHSpdCDPtoFrame: Format %d buffer too short %d needs %d\n",
             format_id, *FrameBytes, cnt);
    return 0;
  }

  //APID   = (pCCSDS->apid_msb<<8) + pCCSDS->apid_lsb;
  SeqNo  = (pCCSDS->pkt_seq_count_msb<<8) + pCCSDS->pkt_seq_count_lsb;
  //PktLen = (pCCSDS->packet_length_msb<<8) + pCCSDS->packet_length_lsb + 7;

  memset(Frame, 0xEE, cnt);

  cout = Frame+16;
  cin =  GSE + sizeof *pEHS + sizeof *pEHSSEC + sizeof *pCCSDS;
  packetend = cin + ((pCCSDS->packet_length_msb<<8) + pCCSDS->packet_length_lsb) + 1;

  for (i=0; i<nPDL; i++)
  {
    status  = (*cin++)<<24;
    status += (*cin++)<<16;
    status += (*cin++)<< 8;
    status += (*cin++);

    count  = (*cin++)<<24;
    count += (*cin++)<<16;
    count += (*cin++)<< 8;
    count += (*cin++);

  //  The ISS HK data typically spans multiple APID 876 packets.  We get one CDP for 
  //  each occurance of an APID 876 packet.  I consider this a bug, HOSC considers it a
  //  feature.  Therefore if any data is stale we reject the entire packet.

    if (((status & 0x40000000) != 0) || (*cin != ' '))
    {
      Frame[0] = ' ';
      Frame[1] = 0;
      return 0;
    }

    if (((cin + pPDL[i].Length) >= packetend) || (count != 1))
    {
      sprintf((char*)Frame, "*EHSpdCDPtoFrame: MSID[%i] Fmt %d length %d"
              " or count %d not valid",
              i, format_id, pPDL[i].Length, count);
      return 0;
    }

    cin++;

    switch(pPDL[i].EntryType)
    {
      case PDL_CTIME:
        Time = (*(cin+0) << 24) + (*(cin+1) << 16) + (*(cin+2) <<  8) + (*(cin+3));
        break;

       case PDL_FTIME:
        Fine =  *(cin);
        break;

      default:
      case PDL_HK:
        memcpy(cout, cin, pPDL[i].Length);
        cout += pPDL[i].Length;
        break;
    }
    cin += pPDL[i].Length;
  }

  if (format_id == 3)
  {
    SeqNo = swap16(((int16*)Frame)[12]);  // Get the copy of SeqNo AMS stashed
  }
  else
  {
    SeqNo = FakeSeqNo++;

    if (HeartBeat != Frame[19])
    {
      if (HeartBeat == (int8)(Frame[19] + 1))
        FakeSeqNo--;
      else
        FakeSeqNo++;
    }
    HeartBeat = Frame[19] + 1;
  }
      
  memset(Frame, 0, 16);

  pCCSDS = (pCCSDS_HEADER) (Frame);
  pCCSDS->version = 0;
  pCCSDS->type = 1;                     // Payload (not core)
  pCCSDS->flag = 1;                     // Secondary header present
  pCCSDS->apid_msb = (int8)(APID>>8);
  pCCSDS->apid_lsb = (int8)(APID);
  pCCSDS->sequence_flag = 3;
  pCCSDS->pkt_seq_count_msb = (int8)(SeqNo>>8);
  pCCSDS->pkt_seq_count_lsb = (int8)(SeqNo);
  pCCSDS->packet_length_msb = (int8)((cnt+16-7)>>8);
  pCCSDS->packet_length_lsb = (int8)((cnt+16-7));

  pCCSDSSEC = (pCCSDS_SECONDARY_HEADER) ((int8*)Frame + sizeof *pCCSDS);
  cout = (int8*) pCCSDSSEC;
  *cout++ = (int8)(Time >> 24);
  *cout++ = (int8)(Time >> 16);
  *cout++ = (int8)(Time >>  8);
  *cout++ = (int8)(Time);
  pCCSDSSEC->fine_time = (int8) Fine;
  pCCSDSSEC->packet_type = 6;
  pCCSDSSEC->zoe_tlm = 0;
  pCCSDSSEC->checkword_indicator = 0;  // Maybe there but would be invalid
  pCCSDSSEC->time_id = 0;
  pCCSDSSEC->version_id = swap16(0xDEAD);
  pCCSDSSEC->cycle_counter = swap16(0xBEEF);

  //  NOTE:  Since the CCSDS header was stripped we dont know if this
  //         Frame had a CheckWord.  FS_CRC is carried thru but any CRC
  //         value would be useless since it was computed over the CCSDS
  //         header.  Hence all frames coming out of this routine have
  //         CheckWord and FS_CRC cleared.  If computed values for these
  //         exist at the end of the buffer they should lay outside any
  //         AMSBlock segment and ignored by deframing routines.

  if (format_id == 7)
    Frame[18] &= ~(FS_CRC<<4);  //  If FS_CRC is present turn it off
  else
    Frame[16] &= ~(FS_CRC<<4);  //  If FS_CRC is present turn it off

  *FrameBytes = cnt + 16;
  return 1;
}

//~----------------------------------------------------------------------------

int EHSpdCDPConfigure(bool RIC, int8 *buff, int32 *Bytes, int *nPDLrtn)
{
int len = 0;
pPDLdefs pPDL;
int32    nPDL;
int cnt;
int i;

  if (RIC)
  {
    pPDL = EHSformat7;
    nPDL = (sizeof EHSformat7) / (sizeof EHSformat7[0]);
  }
  else
  {
    pPDL = EHSpdPDLdefs;
    nPDL = (sizeof EHSpdPDLdefs) / (sizeof EHSpdPDLdefs[0]);
  }

  cnt = 0;
  for (i=0; i<nPDL; i++)
    cnt += pPDL[i].Length;

  if (*Bytes < (4 + (nPDL * 21)))
  {
    LogError("EHSpdCDPConfigure: %d bytes is not enough bytes to hold %d MSIDs\n",
             *Bytes, nPDL);
    return 0;
  }

  memset(buff, 0, (4 + (nPDL * 21)));

  buff[len++] = (int8)(nPDL >> 24);
  buff[len++] = (int8)(nPDL >> 16);
  buff[len++] = (int8)(nPDL >> 8);
  buff[len++] = (int8)(nPDL);

  for (i=0; i< nPDL; i++)
  {
    strcpy((char*)&buff[len], (char*)pPDL[i].RangePCN);
    len += 21;

    buff[len++] = 0;  //??? Try getting ALL samples (1) // Get first sample (0)

    buff[len++] = 0;  // Functions for sample (unprocessed w/o LES)
    buff[len++] = 0; 
    buff[len++] = 0; 
    buff[len++] = 1; 
  }

  *Bytes = len;
  *nPDLrtn = nPDL;
  return 1;
}

//~----------------------------------------------------------------------------

int EHSpdCDPConfigReply(bool RIC, int8 *buff, int32 Bytes, int Debug)
{
int rtn = 1;
pPDLdefs pPDL;
int32    nPDL;
int cnt;
int i, j;
bool match;

  if (RIC)
  {
    pPDL = EHSformat7;
    nPDL = (sizeof EHSformat7) / (sizeof EHSformat7[0]);
  }
  else
  {
    pPDL = EHSpdPDLdefs;
    nPDL = (sizeof EHSpdPDLdefs) / (sizeof EHSpdPDLdefs[0]);
  }

  i = (buff[4]<<8) + buff[5];
  j = (buff[2]<<8) + (buff[3]);
  LogNormal("EHSpdCDPConfigReply: CDP Configuration Reply APID %d.  Config Status %d\n", i, j);

  cnt = 0;
  j = 6;
  for (i=0; i<nPDL; i++)
  {
    match = strcmp((char*)&buff[j], (char*)pPDL[i].RangePCN) == 0;
    if (! match)
      rtn = 0;
    
    LogNormal("%s %-16.16s %-16.16s Status: \'%c\' Offset %4d EHSType %3d "
              "Ntype %3d Max %3d Bytes %3d Bits %3d\n",
              match?"   ":"-->", 
              &buff[j], pPDL[i].RangePCN,
              (buff[j+21]==0)?' ':(buff[j+21]),
              (buff[j+22] << 24) + (buff[j+23] << 16) + (buff[j+24] << 8) + buff[j+25],
              buff[j+26],
              buff[j+27],
              (buff[j+28]<<8) + buff[j+29],
              (buff[j+30]<<8) + buff[j+31],
              (buff[j+32]<<8) + buff[j+33]); 
    j += 21 + 1 + 4 + 1 + 1 + 2 + 2 + 2;

    cnt += pPDL[i].Length;
  }

  return rtn;
}
