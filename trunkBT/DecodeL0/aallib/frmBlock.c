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

//  frmBlock.c   AMS Block Framing Routines

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
//      - Added ByteSwap support for 1553 which does hardware swap
//    Jan 2005 by Peter Dennett
//      - Tuning during FIT at KSC
//    Dec 2007 by Peter Dennett
//      - Updated for MinGW for WIN32 operation
//      - Added support for AMSBlock secondary header 
//    Mar 2008 by Peter Dennett at CERN
//      - Updated AMSBlock secondary header names to match AAL's 
//    Oct 2008 by Peter Dennett at CERN
//      - Usage of LogPrintf
//    Jun 2010 by Peter Dennett at PADSOFT
//      - RIC support for AMS Laptop

#include "aallib.h"
#include "frm.h"
#include "crc.h"
#include "apid.h"
#include "logfile.h"

#ifdef LITTLE_ENDIAN
# include <byteswap.h>
#endif

////////////////////////////////////////////////////////////////////////////
//-Static: frmCCSDS
bool frmCCSDS(pFRAME pFRM, int8 *Frame)
//-Synopsis:    Build a CCSDS header into the frame
//-Returns:     TRUE
//-ARG_I:       pFRM       Object for this instance
//-ARG_I:       Frame      Place to build the frame into 
//-Description: Build a CCSDS header
//-SeeAlso:
//-Includes:
//-Method:
//-Restrictions:
//-Notes:
//-Bugs:
//-Revisions:
//-End:
{
int32 w;
int8 *cp;
int32 Time;
int32 Fine;
int pt;

//  The ISS uses the GPS epoch (Midnight 6-Jan-1980).  This is the number of 
//  seconds later that this epoch begins from the Unix timestamp epoch 
//  (Midnight 1-Jan-1970).

#define ISSEPOCH 315964800

  if (pFRM->Options & FO_EXTCMD)
    pFRM->frmTIME.tv_sec = pFRM->frmTIME.tv_usec = 0;
  else
    timeGet(&pFRM->frmTIME);

  Time = pFRM->frmTIME.tv_sec - ISSEPOCH;
  Fine = pFRM->frmTIME.tv_usec / 6400;

#ifdef FRM_DEBUG
  memset(Frame, 0x33, pFRM->FrameSize);
#endif

  cp = Frame;

//  Build top bits of APID plus TYPE=1 and SECHDR=1 into the first 
//  byte.   Next complete the first 16 bit word with the rest of the APID
//  WORD 1

  *cp++ = (int8)(((pFRM->APID >> 8) & 0x07) | 0x18);
  *cp++ = (int8) pFRM->APID;

//  Put the sequence number info in the next word
//  WORD 2

  w = 0x3;

  *cp++ = (int8)((w << 6) | ((pFRM->Sequence >>8 ) & 0x3F));
  *cp++ = (int8) pFRM->Sequence;
  
//  The packet length takes the next word
//  WORD 3

  w = pFRM->FrameSize - 7;
  *cp++ = (int8)(w >> 8);
  *cp++ = (int8) w;

//  Now start the secondary header with the time since midnight between
//  Jan 5 and 6 1980.  
//  WORD 4
//  WORD 5

  *cp++ = (int8)(Time >> 24);
  *cp++ = (int8)(Time >> 16);
  *cp++ = (int8)(Time >>  8);
  *cp++ = (int8)(Time);

//  Use fine from clock
//  TimeId 01 (data generation)
//  Request check word if applicable
//  Data is not from ZOE (not from comm recorder)
//  xxxPacket type is 0110b = Payload Private/Science
//  Packet type is 0101 = Payload ground command
//  WORD 6

  if (pFRM->Options & FO_RIC)
    pt = 0xA;
  else
    pt = 0x6;

  *cp++ = (int8) Fine;     // Fine time
  *cp++ = (int8) (0x40 | pt | ((pFRM->Options & FO_CHECKWORD)?(1<<5):0));

//  The element and packet IDs 
//  WORD 7
//  WORD 8

#define PID1 0x1111
#define PID2 0x2222

  *cp++ = (int8) (PID1 >> 8);
  *cp++ = (int8) (PID1);

  *cp++ = (int8) (PID2 >> 8);
  *cp++ = (int8) (PID2);

//  Handle PLMDM Health and Status (subset) header.

#define AMS02_SUBSET_ID 308
#define ACOP_SUBSET_ID  309

  if (pFRM->Options & FO_PLMDM)
  {
    if (pFRM->Options & FO_ACOP)
      w = ACOP_SUBSET_ID;
    else
      w = AMS02_SUBSET_ID; // AMS-02 Program assigned SubSetID

    *cp++ = (int8) (w >> 8);
    *cp++ = (int8) (w);

    w = 0x0;             // ServiceRequestID
    *cp++ = (int8) (w >> 8);
    *cp++ = (int8) (w);

    w = 0x0;             // ServiceRequestData
    *cp++ = (int8) (w >> 8);
    *cp++ = (int8) (w);

    w = 0x0;             // CautionWarning
    *cp++ = (int8) (w >> 8);
    *cp++ = (int8) (w);

// X.Cai - Add sequence number here

    *cp++ = (int8)((pFRM->Sequence >>8 ) & 0x3F);
    *cp++ = (int8) pFRM->Sequence;
  }

//  Handle FO_PLMDMCMD which inserts a couple of 16-bit words

  if (pFRM->Options & FO_PLMDMCMD)
  {
    w = 0x6666;          // Reserved
    *cp++ = (int8) (w >> 8);
    *cp++ = (int8) (w);

    w = 0x07F;           // LegalStationMode
    *cp++ = (int8) (w >> 8);
    *cp++ = (int8) (w);

// X.Cai - Add sequence number here

    *cp++ = (int8)((pFRM->Sequence >>8 ) & 0x3F);
    *cp++ = (int8) pFRM->Sequence;
  }

  if (pFRM->Options & FO_RIC_HS)
  {
    w = pFRM->ECW;       // Error and Caution Word (ECW)
    *cp++ = (int8) (w >> 8);
    *cp++ = (int8) (w);

    *cp++ = (int8)(pFRM->Sequence >> 8); // EXPRESS wants 16-bits
    *cp++ = (int8) pFRM->Sequence;
  }

  if (pFRM->Options & FO_RIC)
  {
    w = 0x3333;          // Reserved
    *cp++ = (int8) (w >> 8);
    *cp++ = (int8) (w);

    w = 0x07F;           // LegalStationMode
    *cp++ = (int8) (w >> 8);
    *cp++ = (int8) (w);

    w = pFRM->FrameSize - (16 + 4 + 2 + 2); // EXPRESS Header Message byte count
    *cp++ = (int8) (w >> 8);                // CCSDS Hdr (Rsv, Sta Legal) (CW) (cnt)
    *cp++ = (int8) (w);

    w = 0xBEEF;         // EXPRESS Header Message type
    *cp++ = (int8) (w >> 8);
    *cp++ = (int8) (w);

    w = pFRM->RICsrc;   // EXPRESS Header Source function code (0x0F)
    *cp++ = (int8) (w >> 8);
    *cp++ = (int8) (w);

    w = pFRM->RICtar;           // EXPRESS Header Target function code (0x10)
    *cp++ = (int8) (w >> 8);
    *cp++ = (int8) (w);

#if 0
    *cp++ = (int8)((pFRM->Sequence >>8 ) & 0x3F);
    *cp++ = (int8) pFRM->Sequence;
#endif
  }

// X.Cai - Move to the place after PLMDM words  

  pFRM->Sequence++;
  
//  Insert the frame status bits and set the first block
//  pointer to "fill"

  *cp++ = 0x0F | ((((pFRM->Options & FO_CRC)      ? FS_CRC:0) |
                   ((pFRM->Options & FO_RANDOMIZE)? FS_RANDOMIZE:0) |
                   ((pFRM->Options & FO_CHD)      ? FS_CHD:0) |
                   ((pFRM->Options & FO_EXTCMD)   ? FS_EXTCMD:0)
                                                             ) << 4);
  *cp++ = 0xFF;

  return TRUE;
}

////////////////////////////////////////////////////////////////////////////
//-Extern: frmBlock
bool frmBlock(pFRAME pFRM, int8 *Frame, pAMSBlock Block)
//-Synopsis:  Process an AMS block
//-Returns:     TRUE if block is complete, else FALSE and frame is emitted
//-ARG_I:       pFRM       Object for this instance
//-ARG_I:       Frame      Place to build the frame into 
//-ARG_I:       Block      The AMSBlock to pack into frame(s)
//-Description: Process an AMS block.
//-
//-             This routine must be called repeatly with the same block and
//-             frame until the call returns TRUE indicating the block is 
//-             completely processed.
//-
//-             On a return of FALSE the "Frame" is full with a complete 
//-             CCSDS frame.  The application should process this frame as 
//-             desired and call again to complete the "Block" processing. 
//-             The "Frame" parameter can indicate a new buffer in this case.
//-
//-             If the Block parameter is NULL the CCSDS frame is
//-             flushed.  The return value indicates is a frame was
//-             emitted.
//-
//-             FrameBlock and FrameDeBlock handle byte swapping on
//-             little endian systems (INTEL).  All AMSBlocks are
//-             assumed to be constructed of 16-bit words and are
//-             byte swapped.
//-SeeAlso:
//-Includes:
//-Method:
//-Restrictions:
//-Notes:       Till shown why I shouldn't I am forcing the first data
//-             word to be in the same frame has the block header.
//-Bugs:
//-Revisions:
//-End:
{
int32 w;
int8 *cp;
int hb;

  if (pFRM->DeBlocking)
  {
    Panic("frmBlock: Can not frame and block at same time");
    return TRUE;
  }

#if FO_DEBUG & 0
  if (pFRM->Options & FO_DEBUG)
  {
    LogDebug(0, "frmBlock: +++Entry with Block %p\n", Block);
    
    LogDebug(0, "frmBlock: Frame at entry\n");
    LogHexDump(Frame, pFRM->FrameSize, 1);
  }
#endif

//  If flush requested or our next block is too big to fit...
//  We need to know the length of bytes for header, including
//  the length field

  if ( ! Block)
  {
    hb = 0;
  }
  else
  {
    hb = (Block->DataType>31) ? 4 : 2;
    if ((Block->DataCount + hb) > 0x7FFF)
      hb += 2;
    hb += 2;
#if FO_ABSHEADER
    if (pFRM->Options & FO_ABSHEADER)
      hb += abiIsReply(Block) ? 6 : 2;
#endif
  }

  if (( ! Block) || ((pFRM->Index + hb  >=  pFRM->DataSize)))
  {

//  If requesting a flush and nothing yet in block and we dont
//  need fill frames just return "no frame"

    if (( ! Block) && ( ! pFRM->Index) && (! (pFRM->Options & FO_FILLFRAMES)))
      return TRUE;

    cp = Frame + pFRM->NASAHdrSize;

//  If some data but no block started in frame we need to update the first 
//  pointer.  If no data we need a CCSDS header for the fill frame

    if (pFRM->Index)
    {
      if ( ((*(cp) & 0x0F) == 0x0F) && (*(cp+1) == 0xFF))
        *(cp + 1) = 0xFE;
    }
    else
    {
      frmCCSDS(pFRM, Frame);
    }

//  Insert the end of frame marker if done with block and
//  there is room

    if ((! pFRM->Done) && (pFRM->DataSize > pFRM->Index))
    {
      cp += pFRM->AMSHdrSize + pFRM->Index;
      w = END_OF_FRAME;
      *cp++ = (int8) (w>>8);
      *cp++ = (int8) (w);
      pFRM->Index += 2;

#ifdef FRM_DEBUG
//  Debug Pad end of frame with 0x55 
      if (pFRM->DataSize > pFRM->Index)
      {
        w = pFRM->DataSize - pFRM->Index;
        if (w > 4096)
        {
          LogDebug(0, "frmBlock: prevent memory spray DataSize %ld Index %ld\n",
                 pFRM->DataSize, pFRM->Index);
          fflush(stdout);
          *cp = *((char *)NULL);
        }
#if FO_DEBUG
        if (pFRM->Options & FO_DEBUG)
          LogDebug(0, "frmBlock:     Segment pad at %ld of %ld bytes\n", 
                 pFRM->AMSHdrSize + pFRM->Index, w);
#endif
        if (w)
          memset(cp, 0x55, w);
      }
    }
#endif

    if (pFRM->Options & FO_CHD)
    {
      cp = Frame + pFRM->NASAHdrSize + 2;
      for(w=0; w<sizeof pFRM->CHD; w++)
        *cp++ = pFRM->CHD[w];
    }

#ifdef LITTLE_ENDIAN
    if (pFRM->Options & FO_BYTESWAP)
    {
      int16 *w16;
      int i;

      i = (pFRM->FrameSize/2);
      for (w16=(int16 *)Frame; i>0; i--, w16++)
        *w16 = __bswap_16(*w16);
    }
#endif

    //frmDump(pFRM, Frame, "Before");

    if (pFRM->Options & FO_CRC)
    {
      //LogDebug(0, "frmBlock: CRC Attach %s\n", (pFRM->Options & FO_CHECKWORD)?" (after CW)":""); 
      if (pFRM->Options & FO_CHECKWORD)
        CRC16_Attach(Frame, pFRM->FrameSize - 2);
      else
        CRC16_Attach(Frame, pFRM->FrameSize);
      //frmDump(pFRM, Frame, "CRC   ");
    }

    if (pFRM->Options & FO_RANDOMIZE)
    { 
      //frmDump(pFRM, Frame, "Before");
      frmRandomize(Frame + 18, pFRM->FrameSize - 
                   (18 + ((pFRM->Options & FO_CHECKWORD)?2:0)));
      //frmDump(pFRM, Frame, "Rnd   ");
    }

    if (pFRM->Options & FO_CHECKWORD)
    {
      frmCheckWord(Frame, pFRM->FrameSize, 
                   (pFRM->Options & FO_BYTESWAP)!=0, TRUE);
      //frmDump(pFRM, Frame, "CW    ");
    }

#if FO_DEBUG
    if (pFRM->Options & FO_DEBUG)
    {
      cp = Frame + pFRM->NASAHdrSize;

      w =  *cp++ << 8;
      w |= *cp++;
      w &= 0x0FFF;
      LogDebug(0, "frmBlock: ----Returning frame first: 0x%X size: %ld ~", (int)w, pFRM->FrameSize);

      if (pFRM->Options & FO_CHECKWORD)
      {
        if (pFRM->Options & FO_BYTESWAP)
          w = (*(Frame + pFRM->FrameSize - 1) << 8) + (*(Frame + pFRM->FrameSize - 2));
        else
          w = (*(Frame + pFRM->FrameSize - 1)) + (*(Frame + pFRM->FrameSize - 2) << 8);

        LogDebug(0, " checkword: 0x%lX", w);
      }
      LogDebug(0, "\n");
    }
#endif

//  Mark the "next" frame as empty and pass the frame back

    pFRM->Index = 0;
    return FALSE;
  }

#if FO_DEBUG
  if ((pFRM->Options & FO_DEBUG) && (! pFRM->Index))
      LogDebug(0, "frmBlock: ----Starting new frame\n");
#endif

//  If we are just starting the frame, build a CCSDS header

  if ( ! pFRM->Index)
    frmCCSDS(pFRM, Frame);

//  If this is the start of a new block we need block header 
//  processing

  if (! pFRM->Done)
  {

// See if the first block in the frame, if so set the first
// block pointer

    cp = Frame + pFRM->NASAHdrSize;

//  If no block started in frame we need to update the first 
//  pointer

    if ( ((*(cp) & 0x0F) == 0x0F) && (*(cp+1) == 0xFF))
    {
      w = (pFRM->Index + 1) >> 1;       // X.Cai - Chnaged for word pointer,    w = pFRM->Index;
      *cp   = (*cp & 0xF0) | ((int8)(w >> 8) & 0x3F);
      cp++;
      *cp++ = (int8) w;
    }

    cp = Frame + pFRM->NASAHdrSize + pFRM->AMSHdrSize + pFRM->Index;

#if FO_DEBUG
    if (pFRM->Options & FO_DEBUG)
      LogDebug(0, "frmBlock:  ++Buiding Block Header at %d\n", cp - Frame);
#endif

//  See if we need "format 2" block length

    w = Block->DataCount + 2;
    if (Block->DataType > 31)
      w += 2;

#if FO_ABSHEADER
    if (pFRM->Options & FO_ABSHEADER)
      w += abiIsReply(Block) ? 6 : 2;
#endif

    if (w > 0x7FFF)
    {
      *cp++ = (int8) ((w >> 24) | 0x80);
      *cp++ = (int8)  (w >> 16);
      pFRM->Index += 2;
    }
    *cp++ = (int8) (w >> 8);
    *cp++ = (int8) (w);
    pFRM->Index += 2;

//  Put the block type, node address, and some DataType

    if ((Block->DataType > 31) && ((Block->DataType & 0x1F0000) != 0x1F0000))
      Panic("frmBlock: Invalid DataType 0x%X", Block->DataType);

    w = ((Block->BlockType & 3) << 14) | ((Block->NodeAddress & 0x1FF) << 5);
    if (Block->DataType < 32)
      w |= Block->DataType;
    else
      w |= 0x1F;

    *cp++ = (int8) (w >> 8);
    *cp++ = (int8) (w);
    pFRM->Index += 2;

//  If the DataType is larger then 31 we have an extended type
//  and need another 16 bits. 

    if (Block->DataType > 31)
    {
      w = Block->DataType;
      *cp++ = (int8) (w >> 8);
      *cp++ = (int8) (w);
      pFRM->Index += 2;
    }

#if FO_ABSHEADER
    if (pFRM->Options & FO_ABSHEADER)
    {
      *cp++ = (((Block->CommandTag>>8)&0x0F) | ((Block->Error&0x0F)<<4));
      *cp++ = Block->CommandTag&0xFF;
      pFRM->Index += 2;

      if (abiIsReply(Block))
      {
        if (Block->DataTime == 0)
          Block->DataTime = pFRM->frmTIME.tv_sec;
        *cp++ = (Block->DataTime >> 24);
        *cp++ = (Block->DataTime >> 16);
        *cp++ = (Block->DataTime >>  8);
        *cp++ = (Block->DataTime >>  0);

        pFRM->Index += 4;
      }
    }
#endif

    pFRM->Done = 2;    // A dirty lie - see below
  }

//  Point to where to move the data to

  cp = Frame + pFRM->NASAHdrSize + pFRM->AMSHdrSize + pFRM->Index;

//  See how much room is left.  Note: pFRM->Done includes
//  2 bytes for header, our dirty lie so we can tell we have
//  done the block header

  w = pFRM->DataSize - pFRM->Index;
  if (w < (Block->DataCount - (pFRM->Done-2)))
  {
#if FO_DEBUG
    if (pFRM->Options & FO_DEBUG)
      LogDebug(0, "frmBlock:     Segment from %ld to %ld of %ld bytes first 0x%lX last 0x%lX\n",
             (pFRM->Done-2),
             pFRM->NASAHdrSize + pFRM->AMSHdrSize + pFRM->Index, w, 
             abiDataPtr8(Block)[pFRM->Done-2], abiDataPtr8(Block)[pFRM->Done-2+w-1]);
#endif

    memcpy(cp, abiDataPtr8(Block)+(pFRM->Done-2), w);
    pFRM->Done += w;
    pFRM->Index +=w;

//  Recursive call to finalize and flush

    frmBlock(pFRM, Frame, NULL);

    return FALSE;
  }

  w = Block->DataCount - (pFRM->Done-2);
#if FO_DEBUG
  if (pFRM->Options & FO_DEBUG)
    LogDebug(0, "frmBlock:     Segment from %ld to %ld of %ld bytes first 0x%lX last 0x%lX\n",
           pFRM->Done-2, 
           pFRM->NASAHdrSize + pFRM->AMSHdrSize + pFRM->Index, w, 
           abiDataPtr8(Block)[pFRM->Done-2], abiDataPtr8(Block)[pFRM->Done-2+w-1]);
#endif
  memcpy(cp, abiDataPtr8(Block)+(pFRM->Done-2), w);
  pFRM->Done = 0;
  pFRM->Index += w;

  if ( pFRM->Index & 1 ) pFRM->Index++;         // X.Cai - Added for align to word
  if ( pFRM->Index < pFRM->DataSize ) {
    cp = Frame + pFRM->NASAHdrSize + pFRM->AMSHdrSize + pFRM->Index;
    *cp++ = 0;
    *cp++ = 0;
  }
#if FO_DEBUG & 0
  if (pFRM->Options & FO_DEBUG)
  {
    LogDebug(0, "frmBlock: Frame at TRUE return\n");
    LogHexDump(Frame, pFRM->FrameSize, 1);
  }
#endif

  return TRUE;
}
