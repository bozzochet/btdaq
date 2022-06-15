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

//  hexdump.c   Dump utility

//  Revision History
//    Oct 2003 by Peter Dennett
//      - Initial version in Kemah office 

#include "aallib.h"

////////////////////////////////////////////////////////////////////////////
//-Extern: HexDump
void HexDump(void *Buffer, int32 Bytes, bool ByteSwap)
//-Synopsis:    Dump in hex to stdout
//-Returns:     N/A
//-ARG_I:       Buffer     Buffer to dump
//-ARG_I:       Bytes      Number of bytes to dump
//-ARG_I:       ByteSwap   TRUE if bytes are swapped (little endian)
//-Description: Dump in hex to stdout
//-SeeAlso:
//-Includes:
//-Method:
//-Restrictions:
//-Notes:
//-Bugs:
//-Revisions:
//-End:
{
int i;
int16 *wp = (int16 *) Buffer;
int8 *cp = (int8 *) Buffer;
#define BPL (16*2)   // 2 bytes/word

  for (i=0; i<Bytes; i+=2)
  {
    if ((i % BPL) == 0) 
      printf("%5d: ", i);

    if (ByteSwap)
    {
      printf("%02X",  *cp++); 
      printf("%02X ", *cp++);
    }
    else
      printf("%04X ", *wp++); 

    if ((i % BPL) == (BPL-2)) 
      printf("\n");
  }

  if ((i % BPL) != 0) 
      printf("\n");

  return;
}
