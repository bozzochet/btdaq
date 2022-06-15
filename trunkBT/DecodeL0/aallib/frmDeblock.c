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

//  frmDeblock.c   AMSBlock deframing Routines

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
//      - FO_BYTESWAP support
//    Jan 2005 by Peter Dennett
//        Tuning during FIT at KSC
//    Dec 2007 by Peter Dennett
//      - Updated for MinGW for WIN32 operation
//      - Added support for AMSBlock secondary header 
//    Mar 2008 by Peter Dennett at CERN
//      - Updated AMSBlock secondary header names to match AAL's 
//    May 2008 by Peter Dennett in Kemah
//      - Minor updates from testing
//    Oct 2008 by Peter Dennett at CERN
//      - Usage of LogPrintf
//    Mar 2009 by Peter Dennett in Kemah
//      - Callback support for log messages
//    Dec 2009 by Peter Dennett in Kemah
//      - Support for AMSBlock headers spanning frames
//    Jun 2010 by Peter Dennett at PADSOFT
//      - RIC support for AMS Laptop
//    Sep 2011 by Peter Dennett at PADSOFT
//      - Fixing deframing errors that caused multicast of bad data.
//        Problem was continuing AMSBlocks across frames that had
//        sequence errors.
 
#include "aallib.h"
#include "frm.h"
#include "crc.h"
#include "logfile.h"

#include <stdarg.h>

#ifdef LITTLE_ENDIAN
# include <byteswap.h>
#endif

// Set to zero for no limit 

#define FRM_BLOCK_SIZE_LIMIT 500000

////////////////////////////////////////////////////////////////////////////
void frmLogError(pFRAME pFRM, char *fmt, ...)
{
va_list ap;

  va_start(ap, fmt);

  if ( ! pFRM->ErrorLog)
    LogError2("", fmt, ap);
  else
    pFRM->ErrorLog(pFRM->ErrorLogArg, fmt, ap);

  va_end(ap);
}

////////////////////////////////////////////////////////////////////////////
static void frmAbortPrior(pFRAME pFRM)
{
  if (pFRM->Options & FO_DEBUG)
    LogDebug(0, "frmDeblock: frmAbortPrior\n");

  pFRM->InSync = FALSE;
  pFRM->Index = 0;
  pFRM->Done = 0;
  pFRM->Doing = 0; // Jun08
  pFRM->HdrState = 0; // Sep11
  if (pFRM->Block)
  {
    abiDestruct(pFRM->Block);
    pFRM->Block = NULL;
  }
}

////////////////////////////////////////////////////////////////////////////
// returns: 0 Not done with header yet, end of frame
//          1 Done with header - End of frame marker found
//          2 Done with header - EmptybAMemBlock 
//          3 Done with header - AMSBlock to process yet
//         -1 Header error or out of memory
static int frmProcessHeader(pFRAME pFRM, int8 *Frame)
{
int8 *cp;

  cp = Frame + pFRM->NASAHdrSize + pFRM->AMSHdrSize + pFRM->Index;

#if FO_DEBUG
  if (pFRM->Options & FO_DEBUG)
  {
    LogDebug(0, "frmDeblock: Processing AMSBlock header at: %d "
             "(NASA %d AMS %d) HdrState %d\n", 
             (int)pFRM->Index, (int)pFRM->NASAHdrSize,
             (int)pFRM->AMSHdrSize, (int)pFRM->HdrState);
#if 0
    if (pFRM->Index == 0)
      LogHexDump(Frame, pFRM->FrameSize, TRUE);
#endif
  }
#endif

// Handle spanned headers

  while ((( ! pFRM->Doing) && (pFRM->Index < pFRM->DataSize)) || 
         pFRM->HdrState == 99)
  {
#if FO_DEBUG
#if 0
    if (pFRM->Options & FO_DEBUG)
      LogDebug(0, "frmDeblock: HdrState %d Index %d DataSize %d\n", 
               (int)pFRM->HdrState, (int)pFRM->Index, (int)pFRM->DataSize);
#endif
#endif
    switch (pFRM->HdrState)
    {
      case 0:                   // First length byte
        if (*cp & 0x80)
        {
          pFRM->HdrLength = (*cp++) & ~0x80;
          pFRM->HdrState = 1;
        }
        else
        {
          pFRM->HdrLength = *cp++;
          pFRM->HdrState = 3;
        }
        pFRM->Index++;
        break;

      case 1:                   // Second length byte
        pFRM->HdrLength = (pFRM->HdrLength << 8) | (*cp++);
        pFRM->Index++;
        pFRM->HdrState = 2;
        break; 
         
      case 2:                   // Third length byte
        pFRM->HdrLength = (pFRM->HdrLength << 8) | (*cp++);
        pFRM->Index++;
        pFRM->HdrState = 3;
        break;

      case 3:                   // Fourth length byte
        pFRM->HdrLength = (pFRM->HdrLength << 8) | (*cp++);
        pFRM->Index++;
        pFRM->HdrState = 4;
        
        if ( pFRM->HdrLength == END_OF_FRAME)
        {
#if FO_DEBUG
        if (pFRM->Options & FO_DEBUG)
          LogDebug(0, "frmDeblock: frmProcessHeader found end of frame marker at %d\n",
                   pFRM->Index);
#endif
          pFRM->Index = 0;
          pFRM->Done = 0;
          pFRM->Doing = 0; // Jun08
          pFRM->HdrState = 0; // Sep11
            
          return 1;
        }

        if (pFRM->HdrLength < 2)
        {
          frmLogError(pFRM, "frmDeblock: Block too small %ld\n", pFRM->HdrLength);
          return -1;
        }

//  Allocate an AMSBlock to de-block into.  If we ran out
//  of space declare an error and re-sync to the next block

        pFRM->HdrLength -= 2;

        // Make sure we are cleaning up

        if (pFRM->Block)
        {
          frmLogError(pFRM, "frmDeblock: Logic error block already allocated\n");
          abiDestruct(pFRM->Block);
          pFRM->Block = NULL;
        }

        pFRM->Block = abiConstruct(0, 0, pFRM->HdrLength, 0);
        if ( ! pFRM->Block)
        {
          frmLogError(pFRM, "frmDeblock: Out of memory: Block size of %ld\n", pFRM->HdrLength);
          return -1; 
        }
        break;          

      case 4:                   // First byte of block header - BlockType, NodeAddr, DataType
        //  Get the block header information

        pFRM->HdrWork  = (*cp++) << 8;
        pFRM->Index++;
        pFRM->HdrLength -=2;
        pFRM->HdrState = 5;
        break;

      case 5:                   // Second byte of block header
        pFRM->HdrWork |= (*cp++);
        pFRM->Index++;
        
        pFRM->Block->BlockType    = (pFRM->HdrWork >> 14) & 0x3;
        pFRM->Block->NodeAddress  = (pFRM->HdrWork >>  5) & 0x1FF; 
        pFRM->Block->DataType     = (pFRM->HdrWork)       & 0x1F;
        
        if (pFRM->Block->DataType == 0x1F)
        {
          if (pFRM->Block->DataCount < 2)
          {
            frmLogError(pFRM, "frmDeblock: Block too small ext DT %ld\n",
                        pFRM->Block->DataCount);
            return -1;  // Dont really need to reject frame but we do
          }

          pFRM->Block->DataCount -= 2;
          pFRM->HdrState = 6;
        }
        else
        {
          pFRM->HdrState = 8;
        }
        break;

      case 6:                   // Third byte of block header
        pFRM->HdrWork  = (*cp++) << 8;
        pFRM->Index++;
        pFRM->HdrState = 7;
        break;

      case 7:                   // Fourth byte of block header
        pFRM->HdrWork |= (*cp++);
        pFRM->Index++;
        pFRM->HdrState = 8;
        pFRM->Block->DataType = pFRM->HdrWork | 0x1F0000;
        break;
        
      case 8:                   // Check secondary header
#if FO_ABSHEADER
        //  Deal with AMSBlock Secondary header
        if (pFRM->Options & FO_ABSHEADER)
        {
          if ((pFRM->Block->DataCount < 2) || 
              ((abiIsReply(pFRM->Block)) && (pFRM->Block->DataCount < 6)))
          {
            frmLogError(pFRM, "frmDeblock: Block size problem w/secondary header %d\n",
                        pFRM->Block->DataCount);
            pFRM->ErrorsData++;
            pFRM->HdrState = 99;
          } 
          else
          { 
            pFRM->Block->DataCount -= 2;
            if (abiIsReply(pFRM->Block))
              pFRM->Block->DataCount -= 4;
            
            pFRM->HdrState = 9;
          }
        }
        else
#endif // FO_ABSHEADER
        {
          pFRM->HdrState = 99;
        }
        break;
        
      case 9:                   // First byte of secondary header
        pFRM->HdrWork  = (*cp++) << 8;
        pFRM->Index++;
        pFRM->HdrState = 10;
        break;

      case 10:                  // Second byte secondary header
        pFRM->HdrWork |= (*cp++);
        pFRM->Index++;
        pFRM->Block->CommandTag = pFRM->HdrWork & 0x0FFF;
        pFRM->Block->Error = (pFRM->HdrWork >> 12) &  0xF;

        if (abiIsReply(pFRM->Block))
          pFRM->HdrState = 11;
        else
          pFRM->HdrState = 99;
        break;

      case 11:                  // Third byte of secondary header (replies only)
        pFRM->HdrWork  = *cp++;
        pFRM->Index++;
        pFRM->HdrState = 12;
        break;
        
      case 12:                  // Fourth byte of secondary header (replies only)
        pFRM->HdrWork  = (pFRM->HdrWork << 8) | *cp++;
        pFRM->Index++;
        pFRM->HdrState = 13;
        break;

      case 13:                  // Fifth byte of secondary header (replies only)
        pFRM->HdrWork  = (pFRM->HdrWork << 8) | *cp++;
        pFRM->Index++;
        pFRM->HdrState = 14;
        break;

      case 14:                  // Sixth byte of secondary header (replies only)
        pFRM->HdrWork  = (pFRM->HdrWork << 8) | *cp++;
        pFRM->Index++;
        pFRM->HdrState = 99;
        pFRM->Block->DataTime = pFRM->HdrWork;
        break;

      case 99:                  // Header complete
        pFRM->Done = 0;
        pFRM->Doing = pFRM->Block->DataCount;
        
        pFRM->HdrState = 0;

        if (pFRM->Doing == 0)
        {
#if FO_DEBUG
          if (pFRM->Options & FO_DEBUG)
            LogDebug(0, "frmDeblock: Starting AMSBlock.  Index: %d Doing: (Empty) Type 0x%X\n", 
                     (int)pFRM->Index, (int)pFRM->Block->DataType);
#endif
          return 2;
        }
        else
        {
#if FO_DEBUG
          if (pFRM->Options & FO_DEBUG)
            LogDebug(0, "frmDeblock: Starting AMSBlock. Index: %d Doing: %d type: 0x%X\n", 
                     (int)pFRM->Index, (int)pFRM->Doing,(int)pFRM->Block->DataType);
#endif
          return 3;
        }
          
        break;
    }
  }

  return 0;  // still scanning the AMSBlock header
}

////////////////////////////////////////////////////////////////////////////
//-Extern: frmDeblock
bool frmDeblock(pFRAME pFRM, int8 *Frame, pAMSBlock *Block)
//-Synopsis:  De-Block an AMS Frame
//-Returns: TRUE if a block is returned, else FALSE
//-ARG_I:       pFRM       Object for this instance
//-ARG_I:       Frame      The frame currently being de-blocked.  The contents
//-                        maybe modified by this process. 
//-ARG_U:       Block      Pointers to returned AMSBlocks
//-Description: De-block an AMS Frame.
//-
//-             This routine must be called repeatly with the same frame until 
//-             it returns FALSE indicating the frame is completely processed.
//-
//-             On a return of TRUE a pointer to a DADQMalloc AMSBlock is
//-             returned.  The caller is responsible to DAQDFree the AMSBlock.
//-
//-             The application should process this AMSBlock as desired and call
//-             again, with the same "Frame", to continue its processing.
//-
//-             On a return of FALSE the application should pass in a new
//-             "Frame" for processing.
//-SeeAlso:
//-Includes:
//-Method:
//-Restrictions:
//-Notes:
//-Bugs:
//-Revisions:
//-End:
{
int8 *cp;
int32 Sequence=0x13FFF;
int32 First, w;
int32 SeqFlags;
int i;
int16 APID;
int16 Length;
int rtn;

  pFRM->DeBlocking = TRUE;

//  See if done with an old frame

  if (pFRM->Index >= pFRM->DataSize)
  {
#if FO_DEBUG
    if (pFRM->Options & FO_DEBUG)
      LogDebug(0, "frmDeblock: Done. Index %ld DataSize %ld HdrState\n",
               pFRM->Index, pFRM->DataSize, pFRM->HdrState);
#endif
    pFRM->HdrState = 0; 
    pFRM->Index = 0;

    return FALSE;
  }

//  See if new frame

  if ( ! pFRM->Index)
  {
    if (pFRM->Options & FO_BYTESWAP)
    {
      APID = ((*(Frame+1) & 7) << 8) | (*Frame);
      Sequence = ((*(Frame+2) & 7) << 8) | (*Frame+1);
      SeqFlags = (*(Frame+2)) >> 6;
      Length = ((*(Frame+5) << 8) | (*Frame+4)) + 7;
    }
    else
    {
      APID = ((*Frame & 7) << 8) | (*(Frame+1));
      Sequence = ((*(Frame+2) & 0x3F) << 8) | (*(Frame+3));
      SeqFlags = (*(Frame+1)) >> 6;
      Length = ((*(Frame+4) << 8) | (*(Frame+5))) + 7;
    }

#if FO_DEBUG
    if (pFRM->Options & FO_DEBUG)
      LogDebug(0, "frmDeblock: ----Starting frame APID %d SeqNo %d size: %ld."
               " Index %d HdrState %d Doing %d Done %d InSync %d\n",
               APID, Sequence, Length, pFRM->Index, pFRM->HdrState, 
               pFRM->Doing, pFRM->Done, pFRM->InSync);
#endif

    if (pFRM->APID != APID)
      frmLogError(pFRM, "frmDeblock: Incorrect APID.  Expected: %d Got: %d\n",
                  pFRM->APID, APID);

    if (pFRM->Options & FO_BYTESWAP)
      w = ((*(Frame+5) << 8) | (*(Frame+4)));
    else
      w = ((*(Frame+4) << 8) | (*(Frame+5)));
    if ((w+7) != pFRM->FrameSize)
    {
      frmLogError(pFRM, "frmDeblock: Configured for FrameSize %d CCSDS header says %d\n",
                  pFRM->FrameSize, w+7);
    }

    if (pFRM->Options & FO_BYTESWAP)
      cp = Frame + pFRM->NASAHdrSize + 1;
    else
      cp = Frame + pFRM->NASAHdrSize;

    w = (*cp >> 4);

//  frmBlock, perhaps incorrectly, uses 16-bits sequence numbers and does not
//  properly handle the FO_ bits.  We compensate here by simply ignoring them
//  for RIC health and status

    if ( pFRM->Options & FO_RIC_HS ) 
      w = FS_CHD;

#if FO_DEBUG
    if (pFRM->Options & FO_DEBUG)
      LogDebug(0, "frmDeblock: Frame status 0x%X (index %d NASAHdrSize %d)\n",
               (int)w, (int)(cp - Frame), (int)pFRM->NASAHdrSize);
#endif

//  See if we are psuedo randomized from frame status
//  If so we de-randomize in place-- we warned you in the 
//  document header....  

    if (((w & FS_RANDOMIZE) != 0) &&
        ((pFRM->Options & FO_BYTESWAP) != 0))
      LogWarning("frmDeblock: Bug! the combo of RANDOMIZE and BYTESWAP is broken\n");

    if (w & FS_RANDOMIZE)
    {
#if FO_DEBUG
      if (pFRM->Options & FO_DEBUG)
        LogDebug(0, "frmDeblock: De-randomizing\n");
#endif
      pFRM->Options |= FO_RANDOMIZE;
      if ((pFRM->Options & FO_EXT_RAND) == 0)
        frmRandomize(Frame + pFRM->NASAHdrSize + 2, 
                     pFRM->FrameSize - (pFRM->NASAHdrSize + 2));
    }
    else
    {
      pFRM->Options &= ~FO_RANDOMIZE;
    }

    if (pFRM->Options & FO_BYTESWAP)
      i = *(Frame + 10);
    else
      i = *(Frame + 11);

    if (i & (1<<5))   // See if NASA CheckWord present
    {
      pFRM->Options |= FO_CHECKWORD;
      Sequence = frmCheckWord(Frame, pFRM->FrameSize, 
                              ((pFRM->Options & FO_BYTESWAP) ? TRUE : FALSE), FALSE);
      if (pFRM->Options & FO_BYTESWAP)
        First = (*(Frame + pFRM->FrameSize - 1) << 8) + (*(Frame + pFRM->FrameSize - 2));
      else
        First = (*(Frame + pFRM->FrameSize - 1)) + (*(Frame + pFRM->FrameSize - 2) << 8);
 
#if FO_DEBUG
      if (pFRM->Options & FO_DEBUG)
        LogDebug(0, "frmDeblock: Checkword 0x%lX\n", First);
#endif

      if (First != Sequence)
      {
#if FO_DEBUG
        if (pFRM->Options & FO_DEBUG)
        {
          LogDebug(0, "frmDeblock: CheckWord error. Computed: 0x%04lX Received: 0x%04lX\n",
                   Sequence, First);
          //LogHexDump(Frame, pFRM->FrameSize, TRUE);
        }
#endif
        frmDump(pFRM, Frame, "frmDeblock: CWerr");
        pFRM->ErrorsCheckWord++;
        frmAbortPrior(pFRM);
        return FALSE;
      }
    }
    else
    {
      pFRM->Options &= ~FO_CHECKWORD;
#if FO_DEBUG
      if (pFRM->Options & FO_DEBUG)
        LogDebug(0, "frmDeblock: No CheckWord present\n");
#endif
    }

//  See if this buffer has CRC.  If so check it and reject frames 
//  that are incorrect.

    if (w & FS_CRC)
    {
#if FO_DEBUG
      if (pFRM->Options & FO_DEBUG)
        LogDebug(0, "frmDeblock: CRC on frame\n");
#endif
      pFRM->Options |= FO_CRC;
      if (pFRM->Options & FO_CHECKWORD)
      {
        if (CRC16_Check(Frame, pFRM->FrameSize - 2))
        {
#if FO_DEBUG
          if (pFRM->Options & FO_DEBUG)
            LogDebug(0, "frmDeblock: CRCerr1\n");
#endif
          pFRM->ErrorsCRC++;
          frmAbortPrior(pFRM);
          frmDump(pFRM, Frame, "frmDeblock: CRCerr1");
          return FALSE;
        }
      }
      else
      {
        if (CRC16_Check(Frame, pFRM->FrameSize))
        {
#if FO_DEBUG
          if (pFRM->Options & FO_DEBUG)
            LogDebug(0, "frmDeblock: CRCerr2\n");
#endif
          pFRM->ErrorsCRC++;
          frmAbortPrior(pFRM);
          frmDump(pFRM, Frame, "frmDeblock: CRCerr2");
          return FALSE;
        }
      }
    }
    else
    {
      pFRM->Options &= ~FO_CRC;
    }

#ifdef LITTLE_ENDIAN
    if (pFRM->Options & FO_BYTESWAP)
    {
    int16 *w16;
#if FO_DEBUG
      if (pFRM->Options & FO_DEBUG)
        LogDebug(0, "frmDeblock: Byte swapping frame\n");
#endif
      i = (pFRM->FrameSize/2);
      for (w16=(int16 *)Frame; i>0; i--, w16++)
        *w16 = __bswap_16(*w16);
    }
#endif

    if (w & FS_CHD)
      pFRM->Options |= FO_CHD;
    else
      pFRM->Options &= ~FO_CHD;

    pFRM->AMSHdrSize = 2;
    if (pFRM->Options & FO_CHD)
      pFRM->AMSHdrSize += FRAME_CHD_BYTES;

    pFRM->DataSize = pFRM->FrameSize - (pFRM->NASAHdrSize + pFRM->AMSHdrSize);
    if (pFRM->Options & FO_CHECKWORD)
      pFRM->DataSize -= 2;
    if (pFRM->Options & FO_CRC)
      pFRM->DataSize -= 2;

//  When we start a new frame we do some validation.
//  First check that we like the sequence numbering.

    if      ( pFRM->Options & FO_PLMDM )    cp = Frame + 16 + 8;
    else if ( pFRM->Options & FO_PLMDMCMD ) cp = Frame + 16 + 4;
    else if ( pFRM->Options & FO_RIC )      cp = Frame + 2; // 16 + 12;
    else if ( pFRM->Options & FO_RIC_HS )   cp = Frame + 16 + 4;
    else                                    cp = Frame + 2;
    
    SeqFlags = (*cp) >> 6;
    Sequence = ((*cp++) & 0x3F) << 8;
    Sequence |= *cp++;

//  We are forced to believe the first sequence number.

    if (pFRM->FrameCount == 0)
      pFRM->Sequence = Sequence;

//  All frames should be "UNSEGMENTED" per SSP52050 but
//  we go through the drill anyway...

    switch (SeqFlags)
    {
      default:
        pFRM->ErrorsData++;
        break;

       case 3:   // CCSDS_SEQ_UNSEGMENTED:
         // For now treat as sequenced.... 
         // break;
       case 0:   // CCSDS_SEQ_CONTINUE:
       case 2:   // CCSDS_SEQ_LAST:
        if (Sequence != pFRM->Sequence)
        {	  
          pFRM->ErrorsSeqGap++;

// It did not seem to be a good idea to take the sequence number so seriously,
// plus we discovered the RICsim does not produce good sequence numbers. However
// aborting spanning blocks on sequence errors got rid of lots of errors.

          if ( ! ( pFRM->Options & FO_RIC_HS ) )  //?? Remove if RICsim fixed!
          {
#if FO_DEBUG
            if (pFRM->Options & FO_DEBUG)
              LogDebug(0, "frmDeblock: SeqNo %d SeqFlags %d Expected SeqNo %d\n",
                       (int)Sequence, (int)SeqFlags, (int)pFRM->Sequence);
#endif
            frmAbortPrior(pFRM);
          }
        }
        pFRM->Sequence = (Sequence+1) & 0x3FFF;
        break;

      case 1:    // CCSDS_SEQ_FIRST:
        pFRM->Sequence = (Sequence+1) & 0x3FFF;
        break;
    }

    cp = Frame + pFRM->NASAHdrSize;

    First =  *cp++ << 8;
    First |= *cp++;
    First &= 0x0FFF;

#if FO_DEBUG
    if (pFRM->Options & FO_DEBUG)
      LogDebug(0, "frmDeblock: SeqNo %d SeqFlags %d First Block Ptr: 0x%04X at %d\n",
               (int)Sequence, (int)SeqFlags, (int)First, (int)(cp-Frame)-2);
#endif

//  See if fill frame we can ignore....

    if (First == 0x0FFF)     // Fill frame
    {
#if FO_DEBUG
      if (pFRM->Options & FO_DEBUG)
        LogDebug(0, "frmDeblock: Fill frame ignored\n");
#endif
      pFRM->FillCount++;
      frmAbortPrior(pFRM);  // Not sure this is needed, but who fills?
      return FALSE;
    }

    pFRM->FrameCount++;

//  Skip frames dont have blocks starting in them unless we are continuing a
//  prior block

    if ((First == 0xFFE) && ((! pFRM->InSync))) //?? PAD || (pFRM->Done==0)))
    {
#if FO_DEBUG
      if (pFRM->Options & FO_DEBUG)
        LogDebug(0, "frmDeblock: Skipping frame with no block start when out of sync\n");
#endif
      frmAbortPrior(pFRM);
      return FALSE;
    }

//  Check that our first pointer does not point outside our frame

    if ( First != 0xFFE ) First <<= 1;
    
    if ((First != 0xFFE) && ((First + pFRM->NASAHdrSize) > pFRM->FrameSize))
    {
      frmLogError(pFRM, "frmDeblock: SeqNo %d 1st block pointer out of block: 0x%0X\n",
                  Sequence, First);
      //LogHexDump(Frame, pFRM->FrameSize, TRUE);
      pFRM->ErrorsData++;
      frmAbortPrior(pFRM);
      return FALSE;
    }
    
//  If we have a header spanning into this frame we need to complete it so
//  we can see if the block is correct WRT the first block pointer.

    if (pFRM->HdrState)
    {
#if FO_DEBUG
      if (pFRM->Options & FO_DEBUG)
        LogDebug(0, "frmDeblock: Processing spanning AMSBlock header. Index %d\n",
                 pFRM->Index);
#endif
      rtn = frmProcessHeader(pFRM, Frame);
      switch (rtn)
      {
        case -1:
          pFRM->ErrorsData++;
          frmAbortPrior(pFRM);
          return FALSE;

        case 0:
          frmLogError(pFRM, "frmDeblock: Header cannot span two frames.\n");
          pFRM->ErrorsData++;
          frmAbortPrior(pFRM);
          return FALSE;

        case 2:
          pFRM->Done = 0;
          goto EmptyBlockReturn;
          break;

        case 3:
          break;

        default:
        case 1:
          frmLogError(pFRM, "frmDeblock: Header logic error.\n");
          pFRM->ErrorsData++;
          frmAbortPrior(pFRM);
          return FALSE;
      }
    }
    else
    {
      rtn = 4;
    }

//  Make sure the first block pointer is consistant with our current processing

    w = pFRM->Doing - pFRM->Done + pFRM->Index;

    if ((pFRM->InSync) && (First != 0x0FFE)) // Some block starts in this frame
    {
      if (First != (((w + 1) >> 1) << 1))  // X.Cai - Changed for 16-bit word align
      {
#if FO_DEBUG
        if (pFRM->Options & FO_DEBUG)
          LogDebug(0, "frmDeblock: Block sync error. Rtn %d First %d Index %d Doing %d Done %d\n",
                   rtn, First, pFRM->Index, pFRM->Doing, pFRM->Done);
#endif
        frmLogError(pFRM, "frmDeblock: SeqNo %d Bad 1st block ptr on continue: "
                    "First: %d Left: %d Doing %ld rtn %d\n",
                    Sequence, First, w, pFRM->Doing, rtn);
        pFRM->ErrorsBlockSync++;
        frmAbortPrior(pFRM);
        // return FALSE; // See if we can recover within this block
      }
    }

    if (pFRM->Doing >= FRM_BLOCK_SIZE_LIMIT)
      frmLogError(pFRM, "frmDeBlock: Doing is too big %u\n", pFRM->Doing);

    if (pFRM->Done >= FRM_BLOCK_SIZE_LIMIT)
      frmLogError(pFRM, "frmDeBlock: Done is too big %u\n", pFRM->Done);

//  If we are not in sync look to see if we can recover in this frame.

    if ( ! pFRM->InSync )
    {
      pFRM->Done = 0;
      pFRM->Doing = 0;
      pFRM->HdrState = 0; // Sep11

      if (First == 0x0FFE)   // No block starts
      {
#if FO_DEBUG
        if (pFRM->Options & FO_DEBUG)
          LogDebug(0, "frmDeblock: Out of sync and no block starts\n");
#endif
        pFRM->Index = 0;
        return FALSE;
      }

      pFRM->Index = First;
      if (pFRM->Index >= pFRM->DataSize)
      {
        frmLogError(pFRM, "frmDeblock: SeqNo %d 1st block pointer past end: 0x%0X\n",
                    Sequence, First);
        //LogHexDump(Frame, pFRM->FrameSize, TRUE);
        pFRM->Index = 0;
        pFRM->ErrorsBlockSync++;
        return FALSE;
      }

      pFRM->InSync = TRUE;
    }
  }

//  See if we need to start a new block.  In this case our index points
//  to the block length

  if ( ! pFRM->Doing)
  {
    rtn = frmProcessHeader(pFRM, Frame);
    switch (rtn)
    {
      case -1:
        pFRM->ErrorsData++;
        frmAbortPrior(pFRM);
        return FALSE;

      case 0: // Not done with header but done with frame
        pFRM->Index = 0;
        return FALSE;

      case 1: // Found end of frame marker
        return FALSE;

      case 2:
        pFRM->Done = 0;
        goto EmptyBlockReturn;

      case 3:
        break;

      default:
        frmLogError(pFRM, "frmDeblock: Header logic error.\n");
        pFRM->ErrorsData++;
        frmAbortPrior(pFRM);
        return FALSE;
    }
  }

//  Figure out how much more we need and the how much this
//  frame has.

  w = pFRM->Doing - pFRM->Done;
  if (w > (pFRM->DataSize - pFRM->Index))
    w = pFRM->DataSize - pFRM->Index;

  cp = Frame + pFRM->NASAHdrSize + pFRM->AMSHdrSize + pFRM->Index;
#if FO_DEBUG
  if (pFRM->Options & FO_DEBUG)
    LogDebug(0, "frmDeblock:     Segment from %ld of %ld bytes\n", 
           pFRM->NASAHdrSize + pFRM->AMSHdrSize + pFRM->Index, w);
#endif
  memcpy(abiDataPtr8(pFRM->Block)+(pFRM->Done), cp, w);
  pFRM->Done += w;
  pFRM->Index += w;

//  If we are done with a block give it to the caller

 EmptyBlockReturn:

  *Block = pFRM->Block;

  if (pFRM->Done >= pFRM->Doing)
  {
#if FO_DEBUG
    if (pFRM->Options & FO_DEBUG)
      LogDebug(0, "frmDeblock: Done with AMSBlock: Index: %d Doing %d Done %d\n", 
               (int)pFRM->Index, pFRM->Doing, pFRM->Done);
#endif

    pFRM->Done = 0;
    pFRM->Doing = 0;
    pFRM->Block = NULL;

//  Align to word

    if ( pFRM->Index & 1 ) pFRM->Index++;

    return TRUE;
  }

//  See if done with an old frame, but not the block

  if (pFRM->Index >= pFRM->DataSize)
  {
    pFRM->Index = 0;

#if FO_DEBUG
    if (pFRM->Options & FO_DEBUG)
      LogDebug(0, "frmDeblock: Done with frame\n");
#endif

    return FALSE;
  }

  frmLogError(pFRM, "DeBlock: Spanning block did not complete frame\n");
  return FALSE;
}
