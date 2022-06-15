/*~*~*~*-*~*-*~*-*~*-*~*-*~*-*~*-*~*-*~*-*~*-*~*-*~*-*~*-*~*-*~*-*~*-*~*-*~*~*/
/*                                                                           */
/*                    Copyright 2004 by PADSOFT, Inc.                        */
/*                                                                           */
/*    Developed for use on AMS-2, An International Space Station payload     */
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

#ifndef _FRM_H
#define _FRM_H

//  frm.h   Data framing routines

//  Revision History
//    Apr 2002 by Peter Dennett
//      - Initial version (AALFRM)
//    Sep 2003 by Peter Dennett
//      - Modications for use in payload simulator
//    Jul 2004 by Peter Dennett
//      - Migration from MDCSoftware
//    Jan 2005 by Peter Dennett
//      - Tuning during FIT at KSC
//    Dec 2007 by Peter Dennett
//      - Updated for MinGW for WIN32 operation
//      - Added AMSBlock secondary header support
//    Feb 2009 by Peter Dennett
//      - Improved error handling and logging ability

#define FRM_DEBUG 1           //?? Debug

#include "aallib.h"
#include "abi.h"
#include "aaltime.h"
#include "aalMutex.h"

typedef void (*pFrmErrCB)(void *pErrorArg, char *fmt, va_list ap);

#ifdef OS_WIN32
# define __bswap_16(v) ((((v)&0xFF)<<8)|(((v)&0xff00)>>8))
#endif

//  Define our working area for a framing operation

#define END_OF_FRAME  0
#define FRAME_CHD_BYTES 10

typedef struct _FRAME
{
//  Open time parameters

  int32 APID;
  int32 FrameSize;
  int32 Options;      // Use the FO_ values below

//  Run time settable 

  pFrmErrCB ErrorLog; // Call back used to log errors
                      // default is to use LogError directly
  void *ErrorLogArg;  // Passed to the ErrorLog function
  int32 RICsrc;       // RIC source function
  int32 RICtar;       // RIC target function
  int16 ECW;          // Error and caution word

// Data used to provide CHD in frames

  int8 CHD[FRAME_CHD_BYTES]; // Critical Health Data

//  Time from last frame, used for data times as well

  aalTIMEVAL frmTIME;

//  These are working values

  int32 NASAHdrSize;  // All ISS/STS headers
  int32 AMSHdrSize;   // All AMS headers (1st block ptr & CHD)
  int32 DataSize;     // Data field size

  int32 Index;        // Index into frame
  int32 Done;         // Index into block
  int32 Sequence;     // Next block sequence number
  bool DeBlocking;    // Honest check
  int32 Doing;        // Deblock block len
  bool InSync;        // Not the band
  pAMSBlock Block;    // Area to de-block into

  int32 HdrWork;      // Working areas for spanning AMSBlock headers
  int32 HdrState;
  int32 HdrLength;

//  DeBlocking counts

  int32 ErrorsCRC;
  int32 ErrorsCheckWord;
  int32 ErrorsSeqGap;
  int32 ErrorsBlockSync;
  int32 ErrorsData;
  int32 FrameCount;
  int32 FillCount;
} FRAME, *pFRAME;

#define FO_CRC        (1<<0)  // Frames must have CRC
#define FO_CHECKWORD  (1<<1)  // Frames must have NASA CheckWord
#define FO_FILLFRAMES (1<<2)  // Fill frames required
#define FO_CHD        (1<<3)  // Critical health data required
#define FO_PLMDM      (1<<4)  // PLMDM HK format required 
#define FO_OIU        (1<<5)  // OIU HK format required
#define FO_ACOP       (1<<6)  // ACOP format required
#define FO_RANDOMIZE  (1<<7)  // Perform psuedo randomize
#define FO_INPUT      (1<<8)  // This stream is input (commands in)
#define FO_ABSHEADER  (1<<9)  // Add the AMSBlock secondary header to each block
//#define FO_ABSHEADER  (0)     // On for now
//                              // Note: Set value to 0 to turn off feature
#define FO_PLMDMCMD   (1<<10) // The PLMDM inserts 2 words in our command
#define FO_BYTESWAP   (1<<11) // Byte swap the AMSFrame.  For 1553 interfaces that
                              // preform byte swapping in hardware we need to
                              // have 16-bit values 
#define FO_RIC        (1<<12) // Format frames for RIC COMMANDS
#define FO_EXTCMD     (1<<13) // External command bit to be set in Frame Status.
#define FO_RIC_HS     (1<<14) // Health and status via the RIC
#define FO_EXT_RAND   (1<<15) // On deblock randomization has been done externally

#define FO_DEBUG      (1<<30) // Debug code enable

#define FS_RANDOMIZE  (1<<3)  // Frame status randomize
#define FS_CRC        (1<<2)  // Frame status CRC
#define FS_EXTCMD     (1<<1)  // Frame status external command
#define FS_CHD        (1<<0)  // Frame status AMS secondary header (aka CHD)

#define FRAME_BLOCK_START 16

typedef struct _frmListEnt
{
  struct _frmListEnt *Next;
  int FrameSize;
  int8 Buffer[4096];
  int32 Marker;
} frmListEnt, *pfrmListEnt;

typedef struct _frmListHead
{
  char *TaskName;
  aalMutex_t  BufferMutex;
  pfrmListEnt Next;          // The actual list pointer
  pfrmListEnt Block;         // An entry we are blocking into
  pfrmListEnt Deblock;       // An entry we are deblocking from
  int32 Debug;               // Larger is more
  int32 Marker;
  int32 PDFramesInList;      // Should be diff of below...
  int32 PDFramesAdded;
  int32 PDFramesRemoved;
  int32 PDEntConstructed;
  int32 PDEntDestructed;
} frmListHead, *pfrmListHead;

#define ASSERT_FRMLIST 1

#define frmListEntMarker  0x4567
#define frmListHeadMarker 0x4321

#ifdef  ASSERT_FRMLIST
//static void __inline ASSERT_frmListEnt(pfrmListEnt pENT)
#define ASSERT_frmListEnt(pENT) \
{                                           \
  if (pENT->Marker != frmListEntMarker)     \
    printf("ASSERT_frmListEnt failed in %s at line %d func %s\n", \
          __FILE__, __LINE__, __func__); \
}
//static void __inline ASSERT_frmListHeader(pfrmListHead pFLH)
#define ASSERT_frmListHeader(pFLH) \
{                                               \
  if (pFLH->Marker != frmListHeadMarker)        \
    printf("ASSERT_frmListHeader failed in %s at line %d func %s\n", \
            __FILE__, __LINE__, __func__); \
}
#else
#  define  ASSERT_frmListEnt(pENT)
#  define  ASSERT_frmListHead(pFLH)
#endif

bool frmConstruct(pFRAME pFRM, int32 APID, int32 FrameSize, int32 Options);
bool frmDestruct(pFRAME pFRM);

bool frmCCSDS(pFRAME pFRM, int8 *Frame);
bool frmBlock(pFRAME pFRM, int8 *Frame, pAMSBlock Block);
bool frmDeblock(pFRAME pFRM, int8 *Frame, pAMSBlock *Block);
void frmLogError(pFRAME pFRM, char *fmt, ...);

void frmRandomize(int8 *Frame, int32 Bytes);

int16 frmCRC16(int8 *msg, int16 len);
int16 frmCRCCheck(int8 *msg, int16 len);
int16 frmCRCAttach(int8 *msg, int16 len);

int16 frmCheckWord(int8 *Frame, int FrameSize, bool ByteSwap, bool Apply);

bool frmListHeadConstruct(pfrmListHead pFLH, char *TaskName);
bool frmListHeadDestruct(pfrmListHead pFLH);
pfrmListEnt frmListEntConstruct(pfrmListHead pFLH, int FrameSize);
bool frmListEntDestruct(pfrmListHead pFLH, pfrmListEnt pFLE);
bool frmListAdd(pfrmListHead pFLH, pfrmListEnt pFLE);
pfrmListEnt frmListRemove(pfrmListHead pFLH);
bool frmListEmpty(pfrmListHead pFLH);
bool frmListBlock(pfrmListHead pFLH, pFRAME pFrm, pAMSBlock pRequest);
bool frmListDeblock(pfrmListHead pFLH, pFRAME pFrm, pAMSBlock *pRequest);

bool frmDump(pFRAME pFRM, int8 *Frame, char *Tag);

#endif // _FRM_H
