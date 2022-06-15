/*~*~*~*-*~*-*~*-*~*-*~*-*~*-*~*-*~*-*~*-*~*-*~*-*~*-*~*-*~*-*~*-*~*-*~*-*~*~*/
/*                                                                           */
/*                    Copyright 2008 by PADSOFT, Inc.                        */
/*                                                                           */
/*    Developed for use on AMS-02, An International Space Station payload    */
/*                      searching for missing matter                         */
/*                                                                           */
/*                        All rights reserved                                */
/*                                                                           */
/* Developed by:        Peter Armistead Dennett                              */
/*                            PADSOFT, Inc                                   */
/*                          61 Harbor Lane                                   */
/*                        Kemah, Texas 77565                                 */
/*                          (281) 334 3800                                   */
/*                      http://www.padsoft.com                               */
/*                                                                           */
/*~*~*~*-*~*-*~*-*~*-*~*-*~*-*~*-*~*-*~*-*~*-*~*-*~*-*~*-*~*-*~*-*~*-*~*-*~*~*/

//  egsedatatypes.h   Data types supportted by the EGSE systems

#ifndef _EGSEDATATYPES_H
#define _EGSEDATATYPES_H

//  Revision History
//    Apr 2008 by Peter Dennett
//      - Initial version PADSOFT office
//    Jul 2011 by Peter Dennett
//      - Added host name to whoami
//    Aug 2011 by Peter Dennett
//      - Added instance to whoami and provided interface to set

#define EGSE_HRDLfep    "HRDLfep"
#define EGSE_422fep     "422fep"
#define EGSE_echofep    "echofep"
#define EGSE_canfep     "canfep"
#define EGSE_bcfep      "bcfep"
#define EGSE_MCCfep     "MCCfep"
#define EGSE_HOSCfep    "HOSCfep"
#define EGSE_HOSCsim    "HOSCsim"
#define EGSE_MCCsim     "MCCsim"
#define EGSE_PDSSfep    "PDSSfep"
#define EGSE_PDSSfep2   "PDSSfep2"
#define EGSE_PDSSfep3   "PDSSfep3"
#define EGSE_usbHRDLfep "uHRDLfep"

#define EGSE_DT_WhoAmI       0x1FEED0
#define EGSE_DT_GRSStats     0x1FEED1
#define EGSE_DT_422LinkStats 0x1FEED2
#define EGSE_DT_CHD          0x1FEED3
#define EGSE_DT_DDRS         0x1FEED4
#define EGSE_DT_DDRScmd      0x1FEED5
#define EGSE_DT_PLAYBUTTON   0x1FEED6
#define EGSE_DT_TEXTCMD      0x1FEED7
#define EGSE_DT_TXRATE       0x1FEED8
#define EGSE_DT_CHD10        0x1FEEDA

#define EGSE_DT_PAUSEBUTTON  0x1FEEDB
#define EGSE_DT_PLAYBUTTON2  0x1FEEDC

#define EGSE_DT_HOSCCMDCTL   0x1F0200
#define EGSE_DT_RICCHD10     0x1F0201

typedef struct _EGSE_WhoAmI
{
  char Name[12];
  char Version[12];
  char HostName[12];
  char Instance[12];
} EGSE_WhoAmI, *pEGSE_WhoAmI;

typedef struct _EGSE_422LinkStats
{
  int32 RxFrames;               // Data frames received
  int32 RxFillFrames;           // Fill frames received
  float RxDataRate;             // Rate for just data frames
  float RxTotalRate;            // Rate for fill and data
  int32 RxErrors;               // Error count
  //??  int32 RxSeqDrops;

  int32 TxFrames;               // Data frames transmitted
  int32 TxErrors;               // Error count
  float TxDataRate;             // Rate for just data frames

  double diskfree;              // Bytes of disk available on /Data
  int32 CRCerrs;                // CRC or BCH errors
} EGSE_422LinkStats, *pEGSE_422LinkStats;

typedef struct _EGSE_CHD
{
  char CHD[10];
} EGSE_CHD, *pEGSE_CHD;

typedef struct _EGSE_DDRS
{
  int32 Version;                // Version number of this strucuture
  int32 HeartBeat;              // Update heartbeat
  int32 SignalDetect;           // Signal or light detected
  int32 RxFrames;               // Data frames received
  int32 RxFillFrames;           // Fill frames received
  int32 RxDataRate;             // Rate for just data frames (* 1000)
  int32 RxTotalRate;            // Rate for fill and data (* 1000)
  int32 RxErrors;               // Error count
  //??  int32 RxSeqDrops;

  int32 TxFrames;               // Data frames transmitted
  int32 TxErrors;               // Error count
  int32 TxDataRate;             // Rate for just data frames (* 1000)
  int32 CRCerrs;                // CRC or BCH errors

  int32 FileNumber;             // File ID
  int32 FramesInCnt;            // GRS counts
  int32 FramesWritten;
  int32 FramesInFile;

  int32 PlayButton;             // If non-zero play is pressed
  int32 PlayFileNumber;         // file playing
  int32 PlayFrames;             // Frames for this play

  int32 diskfree;              // MBytes of disk available on /Data
  int32 PlayState;             // Play State per PLAYSTATE_xxxx in play.h
} EGSE_DDRS, *pEGSE_DDRS;

typedef struct _EGSE_DDRScmd
{
  int32 CommandType;            // 0: Reset counters
                                // 1: Play (arg0: Start file id, arg1: End file id)
  int32 Arg0;
  int32 Arg2;
  int32 Arg3;
} EGSE_DDRScmd, *pEGSE_DDRScmd;

void UpdateWhoAmI(pEGSE_WhoAmI pWhoAmI, char VERSION[], char BuildId[]);
void UpdateWhoAmI2(pEGSE_WhoAmI pWhoAmI, char VERSION[], char BuildId[], 
                   char Instance[]);

int egseClear(void);
int egseUpdate(char *path);
pAMSBlock egseProcessRequest(pAMSBlock pRequest);

#ifndef DDRS_STATS_DEFINE
extern
#endif 
  EGSE_DDRS DDRStats;

#endif // _EGSEDATATYPES_H
