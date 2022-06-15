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

//  frmCheckword.c   Compute and optionally apply the NASA CheckWord

//  Revision History
//    Sep 2003 by Peter Dennett
//      - Conversion from PIT software
//    Jul 2004 by Peter Dennett
//      - Added ByteSwap support for 1553 which does hardware swap
//    Jul 2004 by Alexei Lebedev
//      - fixed ChekWord calculation

#include "aallib.h"
#include "frm.h"

#ifdef LITTLE_ENDIAN
# include <byteswap.h>
#endif // LITTLE_ENDIAN
 
////////////////////////////////////////////////////////////////////////////
//-Extern: frmCheckWord
int16 frmCheckWord(int8 *Frame, int FrameSize, bool ByteSwap, bool Apply)
//-Synopsis:    Compute the NASA CCSDS check word for a frame
//-Returns:     The computed check word
//-ARG_I:       Frame     Pointer to the area to perform the calculation on
//-ARG_I:       FrameSize Number of bytes for frame.  The check sum is
//-                       computed on two less bytes then this.
//-ARG_I:       ByteSwap  Store (apply) computed value in reversed byte order
//-ARG_I:       Apply     If TRUE apply the checksum (store in the last two
//-                       bytes of the frame), otherwise just return it.
//-Description: Compute the NASA CCSDS check word for a frame.
//-
//-             Optionlly the computed value can be placed into the frame.
//-SeeAlso:
//-Includes:
//-Method:
//-Restrictions:
//-Notes:
//-Bugs:
//-Revisions:
//-End:
{
int16 cs,*w,w16;
int len, i;

  cs = 0;
  len = (FrameSize - 2) / 2;
  w = (int16 *)Frame;
  for (i=0; i<len; i++) {
    w16 = *w++;
    if (ByteSwap) cs += w16;
    else cs += __bswap_16(w16);
  }

#define Peter 1

#ifdef Peter
  if (Apply)
  {
#ifdef LITTLE_ENDIAN
    if (ByteSwap)
      cs =  __bswap_16(cs);
#endif

    Frame = Frame + FrameSize - 2;
    *Frame++ = (int8) (cs>>8);
    *Frame   = (int8) (cs);
 
#ifdef LITTLE_ENDIAN
    if (ByteSwap)
      cs =  __bswap_16(cs);
#endif
  }
#else
  if (Apply) {
    Frame = Frame + FrameSize - 2;
    *Frame++ = (int8) (cs & 0x00FF);
    *Frame   = (int8) (cs >> 8);
  }
#endif

  return cs;
}
