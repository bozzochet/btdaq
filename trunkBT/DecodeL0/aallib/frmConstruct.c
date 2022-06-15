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

//  frmConstruct.c   AMS Block Framing Routines

//  Revision History
//    Apr 2002 by Peter Dennett
//      - Initial version in Kemah office (AALFRM)
//    Sep 2003 by Peter Dennett
//      - Conversion for use in payload simulator
//      - Overhaul to support formats of the CDH ICD
//    Oct 2003 by Peter Dennett
//      - Clean up and test/debug for CheckWord, CRC, Randomize
//    Nov 2003 by Peter Dennett
//      - The frame routines no longer bytes swap while the local block
//        routines do.
//    Jun 2004 by Peter Dennett
//      - Changed to AMS CRC polynomial
//    Jul 2004 by Peter Dennett
//      - Conversion from MDCSoftware
//    Jan 2005 by Peter Dennett
//        Tuning during FIT at KSC
//    Jun 2010 by Peter Dennett at PADSOFT
//      - RIC support for AMS Laptop

#include "aallib.h"
#include "frm.h"

static int8 TaskName[] = "frmConstruct";

////////////////////////////////////////////////////////////////////////////
//-Extern: frmConstruct
bool frmConstruct(pFRAME pFRM, int32 APID, int32 FrameSize, int32 Options)
//-Synopsis:  Create an object for mapping AMS blocks into CCSDS frames
//-Returns: TRUE if no errors, else FALSE
//-ARG_I:       pFRM       Object for this instance
//-ARG_I:       APID       APID to use for this frame
//-ARG_I:       FrameSize  Number of bytes for CCSDS frame, including 
//-                        its headers.
//-ARG_I:       Options    Controls format options: see FO_ constants
//-Description: Create an object for mapping AMS blocks into CCSDS frames
//-SeeAlso:
//-Includes:
//-Method:
//-Restrictions:
//-Notes:
//-Bugs:
//-Revisions:
//-End:
{
  if (FrameSize & 1)
    return Panic("%s: FrameSize must be even", TaskName);

  memset(pFRM, 0, sizeof *pFRM);

  pFRM->APID = APID;
  pFRM->FrameSize = FrameSize;
  pFRM->Options = Options;

  pFRM->NASAHdrSize = 16;                 // CCSDS headers

  if (pFRM->Options & FO_PLMDM)
    pFRM->NASAHdrSize += 10;              // Subset ID, service, and C&W -- AMSequence

  if (pFRM->Options & FO_PLMDMCMD)
    pFRM->NASAHdrSize += 6;               // ReservedWord and LegalStationMode -- AMSequence

  if (pFRM->Options & FO_RIC_HS)
    pFRM->NASAHdrSize += 4;               // Error Caution and Warning (ECW), H&S counter
                                          // AMSequence
  if (pFRM->Options & FO_RIC)
    pFRM->NASAHdrSize += 12;              // ReservedWord, LegalStationMode,
                                          // EXPRESS Header (ByteCnt, MsgTyp, FC(src) FC (target)

  pFRM->AMSHdrSize = 2;                   // FrameStatus & Pointer

  if (pFRM->Options & FO_CHD)
    pFRM->AMSHdrSize += FRAME_CHD_BYTES ; // Critical health data

//  Make sure the frame is of reasonable size

  if (FrameSize < (pFRM->NASAHdrSize + pFRM->AMSHdrSize + 2))
    return Panic("%s: FrameSize %ld not large enough", TaskName, FrameSize);

  pFRM->DataSize = FrameSize - (pFRM->NASAHdrSize + pFRM->AMSHdrSize);
  if (pFRM->Options & FO_CHECKWORD)
    pFRM->DataSize -= 2;
  if (pFRM->Options & FO_CRC)
    pFRM->DataSize -= 2;

  pFRM->RICsrc = 0x000F;
  pFRM->RICtar = 0x00FE;

  return TRUE;
}

////////////////////////////////////////////////////////////////////////////
//-Extern: frmDestruct
bool frmDestruct(pFRAME pFRM)
//-Synopsis:  Terminte the AMS block handling
//-Returns: TRUE if no errors, else FALSE
//-ARG_I:       pFRM       Object for this instance
//-Description: Terminte the AMS block handling
//-SeeAlso:
//-Includes:
//-Method:
//-Restrictions:
//-Notes:       Caller should perform their own "flush" using
//-             FrameBlock()
//-Bugs:
//-Revisions:
//-End:
{
  if (pFRM->Block)
    aalFree((char *)pFRM->Block);

  memset(pFRM, 0, sizeof *pFRM);

  return TRUE;
}
